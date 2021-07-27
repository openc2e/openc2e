import re


def read_exact(f, n):
    ret = f.read(n)
    assert len(ret) == n
    return ret


def peek_exact(f, n):
    ret = f.peek(n)
    assert len(ret[:n]) == n
    return ret[:n]


def read_line(f):
    buf = b""
    while True:
        c = peek_exact(f, 1)
        if c == b"\r":
            read_exact(f, 1)
            c = peek_exact(f, 1)
            assert c == b"\n"
            read_exact(f, 1)
            break
        if c == b"\n":
            read_exact(f, 1)
            break
        buf += read_exact(f, len(c))
    return buf


with open("map.2er", "rb") as f:
    header = read_line(f)
    print(f"{header=}")
    assert header == b"1 4"
    num_cas = read_line(f)
    assert re.match(rb"^\d+$", num_cas)
    num_cas = int(num_cas)
    print(f"{num_cas=}")
    print()
    for i in range(num_cas):
        print(f"CA #{i}")
        name = read_line(f)
        print(f"{name=}")
        unknown = read_line(f)
        assert re.match(rb"^\d+ \d+ \d+$", unknown)
        unknown = tuple(int(_) for _ in unknown.split(b" "))
        print(f"{unknown=}")
        description = read_line(f)
        print(f"{description=}")
        print()

    # TODO: these seem to be hard coded?
    for room_type in range(16):
        for ca_index in range(20):
            gld = read_line(f)
            assert re.match(rb"^\d+(.\d+)? \d+(.\d+)? \d+(.\d+)?$", gld)
            (gain, loss, diffusion) = tuple(float(_) for _ in gld.split(b" "))
            print(f"{room_type=} {ca_index=} {gain=} {loss=} {diffusion=}")

    map_size = read_line(f)
    assert re.match(rb"^\d+ \d+$", map_size)
    map_size = tuple(int(_) for _ in map_size.split(b" "))
    print(f"{map_size=}")

    brmi = read_line(f)
    assert re.match(rb"^\d+ \d+$", brmi)
    brmi = tuple(int(_) for _ in brmi.split(b" "))
    print(f"{brmi=}")

    num_metarooms = int(read_line(f))
    print(f"{num_metarooms=}")
    for _ in range(num_metarooms):
        metaroom_index = int(read_line(f))
        print(f"{metaroom_index=}")

        addm = read_line(f)
        assert re.match(rb"^\d+ \d+ \d+ \d+$", addm)
        (metaroom_x, metaroom_y, metaroom_width, metaroom_height) = tuple(
            int(_) for _ in addm.split(b" ")
        )
        print(f"{metaroom_x=} {metaroom_y=} {metaroom_width=} {metaroom_height=}")

        num_backgrounds = int(read_line(f))
        print(f"{num_backgrounds=}")
        for _ in range(num_backgrounds):
            background = read_line(f)
            print(f"{background=}")

        metaroom_music = read_line(f)
        print(f"{metaroom_music=}")

        num_rooms = read_line(f)
        assert re.match(rb"^\d+$", num_rooms)
        num_rooms = int(num_rooms)
        print(f"{num_rooms=}")
        print()

        for room_i in range(num_rooms):
            room_index = read_line(f)
            assert re.match(rb"^\d+$", room_index)
            room_index = int(room_index)
            print(f"{room_index=}")

            dimensions = read_line(f)
            assert re.match(rb"^\d+ \d+ \d+ \d+ \d+ \d+$", dimensions)
            (
                x_left,
                x_right,
                y_left_ceiling,
                y_left_floor,
                y_right_ceiling,
                y_right_floor,
            ) = [int(_) for _ in dimensions.split(b" ")]
            print(
                f"{x_left=} {x_right=} {y_left_ceiling=} {y_left_floor=} {y_right_ceiling=} {y_right_floor=}"
            )

            room_type = read_line(f)
            assert re.match(rb"^\d+$", room_type)
            room_type = int(room_type)
            print(f"{room_type=}")

            room_music = read_line(f)
            print(f"{room_music=}")
            print()

    num_doors = int(read_line(f))
    print(f"{num_doors=}")
    for _ in range(num_doors):
        room_id1, room_id2 = read_line(f).split(b" ")
        print(f"{room_id1=} {room_id2=}")

        unknown_always_one = read_line(f)
        assert unknown_always_one == b"1"
        print(f"{unknown_always_one=}")

        permeability = int(read_line(f))
        print(f"{permeability=}")
        print()

    assert f.read() == b""
