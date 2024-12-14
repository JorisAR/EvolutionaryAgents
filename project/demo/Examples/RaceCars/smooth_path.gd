@tool
extends Path3D

@export var make_track_smooth: bool:
	get:
		return false
	set(value):
		smooth()

@export var make_track_flat: bool:
	get:
		return false
	set(value):
		flatten()

func flatten():
	if curve == null:
		printerr("Path curve is not defined.")
		return
		
	for i in range(curve.get_point_count()):
		var p = curve.get_point_position(i);
		curve.set_point_position(i, Vector3(p.x, 0, p.z));
		var in_control = curve.get_point_in(i);
		curve.set_point_in(i, Vector3(in_control.x, 0, in_control.z));
		var out_control = curve.get_point_out(i);
		curve.set_point_out(i, Vector3(out_control.x, 0, out_control.z));

func smooth():
	if curve == null:
		printerr("Path curve is not defined.")
		return
	
	# Collect unique points from the curve
	var points = []
	var point_set = {}
	for i in range(curve.get_point_count()):
		var point = curve.get_point_position(i)
		var key = Vector3(round(point.x), round(point.y), round(point.z))
		if key not in point_set:
			point_set[key] = true
			points.append(point)

	# Adjust in and out control points to smooth the path
	var in_controls = []
	var out_controls = []

	for i in range(len(points)):
		var p0 = points[(i - 1 + len(points)) % len(points)]
		var p1 = points[i]
		var p2 = points[(i + 1) % len(points)]
		
		# Calculate tangents
		var tangent = (p2 - p0).normalized()
		
		# Calculate control points based on tangents
		var alpha = 0.5  # Smoothing factor
		var in_control = p1 - alpha * tangent * (p2 - p1).length() / 2.0
		var out_control = p1 + alpha * tangent * (p2 - p1).length() / 2.0
		
		in_controls.append(in_control)
		out_controls.append(out_control)

	# Clear existing points and add smoothed points with new control points
	curve.clear_points()
	for i in range(len(points)):
		curve.add_point(points[i], in_controls[i] - points[i], out_controls[i] - points[i])
		
	curve.add_point(points[0], in_controls[0] - points[0], out_controls[0] - points[0])

	print("Path smoothed and closed.")
