import math

# radius of swerve drive base in meters
radius = 1

# vectors of swerve drive, using [speed (m/s), orientation (deg)] in global reference frame
swerve = [0,0]

# vectors of motors in global frame
m1 = [0,0]
m2 = [0,0]
m3 = [0,0]

# add two vectors
def add_two_vec(vec1, vec2):
    print(vec1,vec2)
    # get difference between two vectors, treating one vector as perpendicular to x axis
    theta_diff = vec2[1] - vec1[1]
    theta_diff = math.radians(theta_diff)
    print(theta_diff)

    # since vec1 is x axis, vector 1 contributes in only the x axis
    # when breaking into components, just add vec2 in x to vec1 magnitude to get x
    # vec2 in y to get y
    # sqrt(a^2+b^2) to get magnitude, and arctan(y/x) to get angle relative to x (add to vec1 orientation)
    x_comp = vec1[0] + math.cos(theta_diff) * vec2[0]
    print(x_comp)
    y_comp = math.sin(theta_diff) * vec2[0]
    print(y_comp)
    f_mag = math.sqrt(x_comp**2 + y_comp**2)
    print(f_mag)
    f_angle = math.atan2(y_comp, x_comp) + math.radians(vec1[1])
    print(f_angle)
    f_angle = math.degrees(f_angle)
    print(f_angle)
    if f_angle < -180:
        f_angle += 360
    elif f_angle > 180:
        f_angle -= 360
    return [f_mag, f_angle]


# input velocity [speed (m/s), orientation (deg)] in local reference frame
# rotation (rad/s)
def convert(velocity, rotation):

    # the vector for each motor in global reference frame is given by adding 
    # 1) velocity (relative to global reference frame)
    # 2) rotation vector (vector perpendicular to each motor relative to the body reference frame)
    #    because the frame is circular, magnitude of vector is calculated by V = rw (formula for tangential speed relative to radius and rad/s)

    # setting all vectors to velocity relative to local reference frame
    m1 = [velocity[0], velocity[1]]
    m2 = [velocity[0], velocity[1]-120]
    m3 = [velocity[0], velocity[1]-240]

    # rotation vector relative to global reference
    # three motors, with motor 1 having an offset of 0 relative to frame
    # because rotation vectors must be perpendicular, add 90 to each
    # to convert from local to global, add orientation of frame in global reference frame
    # rot_ang_m1 = 0+90+swerve[1]
    # rot_ang_m2 = 120+90+swerve[1]
    # rot_ang_m3 = 240+90+swerve[1]

    # create magnitude for each vector
    rot_mag = rotation * radius
    dir = 1

    if rot_mag < 0:
        dir *= -1
        rot_mag *= -1
    print(rot_mag)
    # add two vectors (in local frame) based on direction and magnitude
    m1 = add_two_vec(m1, [rot_mag, 90*dir])
    m2 = add_two_vec(m2, [rot_mag, 90*dir])
    m3 = add_two_vec(m3, [rot_mag, 90*dir])

    print(m1, m2, m3)

convert([10,0],0)