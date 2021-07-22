PGNS = [
    61444,
    61443,
    61450,
    64948,
    64946,
    64947,
    65170,
    65251,
    65266,
    65270,
    65247,
    65265,
    64892
]


def get_candump_filter(can_id: int):
    return hex(can_id << 8) + ':' + '0x00ffff00'

def main():
    print(','.join([get_candump_filter(pgn) for pgn in PGNS]))

if __name__ == '__main__':
    main()
