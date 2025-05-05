import dataclasses
from dataclasses import dataclass
from itertools import repeat
from typing import Optional


@dataclass
class Circuit:
    num: int
    space_num: Optional[int]
    zone_num: Optional[int]

@dataclass
class Preset:
    num: int
    """Map rack space nums to fade times in seconds"""
    fade_time: dict[int, int] = dataclasses.field(default=None)
    """Map rack circuit nums to levels 0-255"""
    levels: dict[int, int] = dataclasses.field(default=None)

    def __post_init__(self):
        if self.fade_time is None:
            self.fade_time = {k: v for k, v in zip(range(1, 17), repeat(0, 16))}
        if self.levels is None:
            self.levels = {k: v for k, v in zip(range(1, 49), repeat(0, 48))}

def optional_int(val: Optional) -> Optional[int]:
    if isinstance(val, str) and len(val) == 0:
        return None
    try:
        return int(val)
    except ValueError:
        return None