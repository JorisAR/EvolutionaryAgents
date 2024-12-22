class_name SplatRenderer extends Node

#region DATA CLASSES
#class Splat:
	#var color : Color
	#var position : Vector2
	#var rotation : float
	#var scale : Vector2
#
	#static func create(c: Color, p: Vector2, r: float, s: Vector2) -> Splat:
		#var splat = Splat.new()
		#splat.color = c
		#splat.position = p
		#splat.rotation = r
		#splat.scale = s
		#return splat
	#
	#func get_bytes() -> PackedByteArray:
		#var array = PackedByteArray()
		#array.resize(SPLAT_SIZE);
		#array.encode_float(0, color.r);
		#array.encode_float(4, color.g);
		#array.encode_float(8, color.b);
		#array.encode_float(12, color.a);
		#array.encode_float(16, position.x);
		#array.encode_float(20, position.y);
		#array.encode_float(24, rotation);
		#array.encode_float(28, scale.x);
		#array.encode_float(32, scale.y);
		#return array
#
	#func float_to_bytes(array: PackedByteArray, value: float, offset: int) -> void:
		#array.encode_float(offset, value);

func get_float_zero_bytes() -> PackedByteArray:
	var array = PackedFloat32Array()
	array.append(0.0);
	return array.to_byte_array();
	
	
func get_settings_bytes() -> PackedByteArray:
	var array = PackedFloat32Array()
	array.append(_renderResolution.x);
	array.append(_renderResolution.y);
	array.append(SPLAT_COUNT);
	return array.to_byte_array();

#endregion

var SPLAT_COUNT := 128;
const SPLAT_FLOAT_COUNT := 5;
const SPLAT_SIZE := SPLAT_FLOAT_COUNT * 4;


@export_group("Settings")
@export var render_period = 100;
@export var _dataTexture : Texture2D
@export var _renderResolution : Vector2i = Vector2i(512, 512);

@export_group("Requirements")
@export_file() var _computeShader : String
@export var _renderer : TextureRect

@export_group("Output", "output_")
@export var output_store_images : bool;
@export_dir var output_dir : String;

var _rd : RenderingDevice

var _inputTexture : RID
var _outputTexture : RID
var _splatStorageBuffer : RID
var _errorStorageBuffer : RID
var _settingsStorageBuffer : RID
var _uniformSet : RID
var _shader : RID
var _pipeline : RID

var _rids : Array[RID];

var _bindings : Array[RDUniform] = []

var _inputImage : Image
var _outputImage : Image
var _renderTexture : ImageTexture

var _inputFormat : RDTextureFormat
var _outputFormat : RDTextureFormat

var _textureUsage :=\
 RenderingDevice.TEXTURE_USAGE_STORAGE_BIT |\
 RenderingDevice.TEXTURE_USAGE_CAN_UPDATE_BIT |\
 RenderingDevice.TEXTURE_USAGE_CAN_COPY_FROM_BIT |\
 RenderingDevice.TEXTURE_USAGE_SAMPLING_BIT

#region MAIN LOOP
func _ready() -> void:
	create_and_validate_images()
	setup_compute_shader()
	#var splat_bytes = PackedByteArray();
	#for i in range(0,SPLAT_COUNT):
		#var splat = Splat.create(Color(randf(), randf(), randf(), randf() * 0.1), Vector2(randf(), randf()), 0, Vector2(0.5 * randf(), 0.5 * randf()))
		#splat_bytes.append_array(splat.get_bytes());
	#compute_fitness(splat_bytes)
	
func _notification(what : int) -> void:
	if what == NOTIFICATION_WM_CLOSE_REQUEST or what == NOTIFICATION_PREDELETE:
		cleanup_gpu()

#endregion

#region IMAGE SETUP
func create_and_validate_images() -> void:
	_outputImage = Image.create(_renderResolution.x, _renderResolution.y, false, Image.FORMAT_RGBA8)
	_inputImage = _dataTexture.get_image()
	_renderTexture = ImageTexture.create_from_image(_outputImage)
	_renderer.texture = _renderTexture;
#endregion

#region SHADER SETUP
func create_rendering_device() -> void:
	_rd = RenderingServer.create_local_rendering_device()
	
func create_shader() -> void:
	var shaderFile : RDShaderFile = load(_computeShader)
	var spirV := shaderFile.get_spirv()
	_shader = _rd.shader_create_from_spirv(spirV)
	
	
func create_pipeline() -> void:
	_pipeline = _rd.compute_pipeline_create(_shader)
	
func default_texture_format(size: Vector2i) -> RDTextureFormat:
	var format := RDTextureFormat.new()
	format.texture_type = RenderingDevice.TEXTURE_TYPE_2D
	format.width = size.x
	format.height = size.y
	format.format = RenderingDevice.DATA_FORMAT_R8G8B8A8_UNORM
	format.usage_bits = _textureUsage	
	return format

func create_texture_and_uniform(image: Image, format : RDTextureFormat, binding : int) -> RID:
	var view := RDTextureView.new()
	var data := [image.get_data().duplicate()]
	var texture := _rd.texture_create(format, view, data)
	
	var uniform := RDUniform.new()
	uniform.uniform_type = RenderingDevice.UNIFORM_TYPE_IMAGE
	uniform.binding = binding
	_rids.push_back(texture);	
	uniform.add_id(texture)
	_bindings.append(uniform)
	return texture
	
func create_sampler2d_and_uniform(image: Image, format: RDTextureFormat, binding: int) -> RID:
	var view := RDTextureView.new()
	var data := [image.get_data().duplicate()]
	var texture := _rd.texture_create(format, view, data)
	_rids.push_back(texture);
	
	var state = RDSamplerState.new();	
	var sampler := _rd.sampler_create(state);
	_rids.push_back(sampler);
	
	var uniform := RDUniform.new()
	uniform.uniform_type = RenderingDevice.UNIFORM_TYPE_SAMPLER_WITH_TEXTURE
	uniform.binding = binding
	
	uniform.add_id(sampler)
	uniform.add_id(texture)
	_bindings.append(uniform)
	return texture

func create_storage_uniform(data: PackedByteArray, binding: int):
	var buffer = _rd.storage_buffer_create(data.size(), data);
	var uniform := RDUniform.new()
	uniform.uniform_type = RenderingDevice.UNIFORM_TYPE_STORAGE_BUFFER
	uniform.binding = binding	
	uniform.add_id(buffer)
	_bindings.append(uniform)
	return buffer

func update_storage_uniform(uniform: RID, data: PackedByteArray):
	_rd.buffer_update(uniform, 0, data.size(), data);

func create_uniforms() -> void:
	_inputFormat = default_texture_format(_dataTexture.get_size());
	_outputFormat = default_texture_format(_renderResolution);
	_inputTexture = create_sampler2d_and_uniform(_inputImage, _inputFormat, 0)
	_outputTexture = create_texture_and_uniform(_outputImage, _outputFormat, 1)
	var data = PackedByteArray();
	data.resize(SPLAT_COUNT * SPLAT_SIZE);
	_splatStorageBuffer = create_storage_uniform(data, 2)
	_errorStorageBuffer = create_storage_uniform(get_float_zero_bytes(), 3)
	_settingsStorageBuffer = create_storage_uniform(get_settings_bytes(), 4)
	_uniformSet = _rd.uniform_set_create(_bindings, _shader, 0)

func setup_compute_shader() -> void:
	create_rendering_device()
	create_shader()
	create_pipeline()
	create_uniforms()
	
#endregion

#region PROCESSING

func update() -> void:
	if _rd == null:
		return
	var computeList = _rd.compute_list_begin()
	_rd.compute_list_bind_compute_pipeline(computeList, _pipeline)
	_rd.compute_list_bind_uniform_set(computeList, _uniformSet, 0)
	_rd.compute_list_dispatch(computeList, 32, 32, 1)
	_rd.compute_list_end()
	_rd.submit()

var gen = 0;
var g = 0;
var best_fitness = -1e9;
var best_bytes;
func render() -> float:
	if _rd == null:
		return 0.0
	gen += 1
	g += 1
	_rd.sync();
	
	var error_buffer = _rd.buffer_get_data(_errorStorageBuffer, 0, 4)
	#take negative error, as we want to maximise. 
	# (lower error is better, thus higher negative error is better)
	var fitness = -error_buffer.decode_float(0) 
	
	if fitness > best_fitness:
		best_bytes = _rd.texture_get_data(_outputTexture, 0)
		best_fitness = fitness
	
	if gen >= render_period:
		best_fitness = -1e9
		_outputImage.set_data(_renderResolution.x, _renderResolution.y, false, Image.FORMAT_RGBA8, best_bytes)
		_renderTexture.update(_outputImage)
		gen = 0;
		if output_store_images:
			var file := output_dir + "/image_" + str(g) + ".png"
			_renderTexture.get_image().save_png(file)

	return fitness

	
func cleanup_gpu() -> void:
	if _rd == null:
		return
		
	for rid in _rids:
		_rd.free_rid(rid);
		
	_rd.free_rid(_pipeline)
	_rd.free_rid(_shader)
		
	_rd.free()
	_rd = null

#endregion

#region API

func compute_fitness(bytes: PackedByteArray) -> float:
	if _rd == null:
		return -1e9;
	update_storage_uniform(_splatStorageBuffer, bytes);
	update_storage_uniform(_errorStorageBuffer, get_float_zero_bytes());
	update();
	var error = render();
	
	return error;

#endregion
