import time, os, sys
from media.sensor import *
from media.display import *
from media.media import *

# ====================== 基础配置 ======================
PICTURE_WIDTH = 400
PICTURE_HEIGHT = 240
DISPLAY_MODE = "LCD"

if DISPLAY_MODE == "VIRT":
    DISPLAY_WIDTH = ALIGN_UP(1920, 16)
    DISPLAY_HEIGHT = 1080
elif DISPLAY_MODE == "LCD":
    DISPLAY_WIDTH = 640
    DISPLAY_HEIGHT = 480
else:
    raise ValueError("未知的显示模式")

# ====================== 七段数字识别配置 ======================
# 单测时建议画面里只放一个七段数字。
# 如果数字位置固定，把 DIGIT_ROI 改成 (x, y, w, h)，识别会比自动框选稳定。
DIGIT_ROI = None

# 阈值模式：
# "RED" / "GREEN" / "BLUE"：识别彩色亮段
# "DARK"：识别黑色段或黑色印刷七段数字
# "ALL_COLOR"：红绿蓝都尝试
DIGIT_MODE = "ALL_COLOR"

RED_THRESHOLD = (0, 66, 7, 127, 3, 127)
GREEN_THRESHOLD = (42, 100, -128, -17, 6, 66)
BLUE_THRESHOLD = (43, 99, -43, -4, -56, -7)
DARK_THRESHOLD = (0, 45, -30, 30, -30, 30)

# 单个小亮段的最小面积，滤掉小噪声。
MIN_SEGMENT_AREA = 20

# 自动框选时给数字外框留一点边距。
AUTO_PAD = 6

# 每个段位中，亮像素比例超过这个值就认为该段亮。
SEGMENT_ON_RATIO = 0.16

# 如果匹配结果和标准数字差异超过这个值，就输出 ?。
MAX_ACCEPT_DIFF = 1

# 七段顺序：A, B, C, D, E, F, G
SEGMENT_DIGITS = {
    (1, 1, 1, 1, 1, 1, 0): "0",
    (0, 1, 1, 0, 0, 0, 0): "1",
    (1, 1, 0, 1, 1, 0, 1): "2",
    (1, 1, 1, 1, 0, 0, 1): "3",
    (0, 1, 1, 0, 0, 1, 1): "4",
    (1, 0, 1, 1, 0, 1, 1): "5",
    (1, 0, 1, 1, 1, 1, 1): "6",
    (1, 1, 1, 0, 0, 0, 0): "7",
    (1, 1, 1, 1, 1, 1, 1): "8",
    (1, 1, 1, 1, 0, 1, 1): "9",
}


def get_thresholds():
    if DIGIT_MODE == "RED":
        return [RED_THRESHOLD]
    if DIGIT_MODE == "GREEN":
        return [GREEN_THRESHOLD]
    if DIGIT_MODE == "BLUE":
        return [BLUE_THRESHOLD]
    if DIGIT_MODE == "DARK":
        return [DARK_THRESHOLD]
    return [RED_THRESHOLD, GREEN_THRESHOLD, BLUE_THRESHOLD]


def clamp_roi(roi):
    x, y, w, h = roi
    x = max(0, min(PICTURE_WIDTH - 1, int(x)))
    y = max(0, min(PICTURE_HEIGHT - 1, int(y)))
    w = max(1, min(PICTURE_WIDTH - x, int(w)))
    h = max(1, min(PICTURE_HEIGHT - y, int(h)))
    return (x, y, w, h)


def blob_pixels(blob):
    try:
        return blob.pixels()
    except Exception:
        return blob.area()


def find_digit_roi(img, thresholds):
    if DIGIT_ROI is not None:
        return clamp_roi(DIGIT_ROI)

    blobs = img.find_blobs(thresholds, area_threshold=MIN_SEGMENT_AREA, merge=False)
    if not blobs:
        return None

    x1 = PICTURE_WIDTH
    y1 = PICTURE_HEIGHT
    x2 = 0
    y2 = 0
    valid_count = 0

    for b in blobs:
        if b.area() < MIN_SEGMENT_AREA:
            continue
        x, y, w, h = b.rect()
        x1 = min(x1, x)
        y1 = min(y1, y)
        x2 = max(x2, x + w)
        y2 = max(y2, y + h)
        valid_count += 1

    if valid_count == 0:
        return None

    return clamp_roi((x1 - AUTO_PAD, y1 - AUTO_PAD,
                      x2 - x1 + AUTO_PAD * 2, y2 - y1 + AUTO_PAD * 2))


def segment_roi(base_roi, segment_name):
    x, y, w, h = base_roi

    # 七段 ROI 不是精准描边，而是取每个段位的统计区域。
    # 统计区域偏厚一点，能容忍拍摄角度和数码管粗细变化。
    thick_w = max(4, w // 4)
    thick_h = max(4, h // 7)
    mid_y = y + h // 2 - thick_h // 2

    if segment_name == "A":
        return (x + thick_w // 2, y, w - thick_w, thick_h)
    if segment_name == "B":
        return (x + w - thick_w, y + thick_h // 2, thick_w, h // 2 - thick_h)
    if segment_name == "C":
        return (x + w - thick_w, y + h // 2, thick_w, h // 2 - thick_h)
    if segment_name == "D":
        return (x + thick_w // 2, y + h - thick_h, w - thick_w, thick_h)
    if segment_name == "E":
        return (x, y + h // 2, thick_w, h // 2 - thick_h)
    if segment_name == "F":
        return (x, y + thick_h // 2, thick_w, h // 2 - thick_h)
    return (x + thick_w // 2, mid_y, w - thick_w, thick_h)


def segment_on_ratio(img, thresholds, roi):
    x, y, w, h = roi
    if w <= 0 or h <= 0:
        return 0

    blobs = img.find_blobs(thresholds, roi=roi, area_threshold=2, merge=True, margin=1)
    pixels = 0
    for b in blobs:
        pixels += blob_pixels(b)
    return pixels / (w * h)


def match_digit(states):
    exact = SEGMENT_DIGITS.get(states)
    if exact is not None:
        return exact, 0

    best_digit = "?"
    best_diff = 8
    for pattern, digit in SEGMENT_DIGITS.items():
        diff = 0
        for i in range(7):
            if pattern[i] != states[i]:
                diff += 1
        if diff < best_diff:
            best_diff = diff
            best_digit = digit

    return best_digit, best_diff


def detect_digit(img):
    thresholds = get_thresholds()
    digit_roi = find_digit_roi(img, thresholds)
    if digit_roi is None:
        return "NONE"

    img.draw_rectangle(digit_roi, color=(255, 128, 0), thickness=2)

    states = []
    ratios = []
    for name in ("A", "B", "C", "D", "E", "F", "G"):
        roi = clamp_roi(segment_roi(digit_roi, name))
        ratio = segment_on_ratio(img, thresholds, roi)
        is_on = 1 if ratio >= SEGMENT_ON_RATIO else 0
        states.append(is_on)
        ratios.append(ratio)

        color = (0, 255, 0) if is_on else (255, 0, 0)
        img.draw_rectangle(roi, color=color, thickness=1)

    digit, diff = match_digit(tuple(states))
    print("digit:", digit, "diff:", diff, "segments:", tuple(states), "ratios:", ratios)

    if diff > MAX_ACCEPT_DIFF:
        return "?"
    return digit


def init_sensor():
    sensor = Sensor(id=2)
    sensor.reset()
    sensor.set_framesize(width=PICTURE_WIDTH, height=PICTURE_HEIGHT, chn=CAM_CHN_ID_0)
    sensor.set_pixformat(Sensor.RGB565, chn=CAM_CHN_ID_0)
    return sensor


def init_display():
    if DISPLAY_MODE == "VIRT":
        Display.init(Display.VIRT, width=DISPLAY_WIDTH, height=DISPLAY_HEIGHT, fps=60)
    elif DISPLAY_MODE == "LCD":
        Display.init(Display.ST7701, width=DISPLAY_WIDTH, height=DISPLAY_HEIGHT, to_ide=True)


def main():
    try:
        sensor = init_sensor()
        init_display()
        MediaManager.init()
        sensor.run()

        x_offset = (DISPLAY_WIDTH - PICTURE_WIDTH) // 2
        y_offset = (DISPLAY_HEIGHT - PICTURE_HEIGHT) // 2

        while True:
            os.exitpoint()
            img = sensor.snapshot(chn=CAM_CHN_ID_0)

            digit = detect_digit(img)
            img.draw_string(10, 10, "DIGIT: %s" % digit, color=(255, 255, 0), scale=3)
            img.draw_string(10, 45, "MODE: %s" % DIGIT_MODE, color=(0, 255, 255), scale=2)

            Display.show_image(img, x=x_offset, y=y_offset)

    except KeyboardInterrupt:
        print("用户中断")
    except Exception as e:
        print("错误:", e)
    finally:
        if 'sensor' in locals() and isinstance(sensor, Sensor):
            sensor.stop()
        Display.deinit()
        os.exitpoint(os.EXITPOINT_ENABLE_SLEEP)
        time.sleep_ms(100)
        MediaManager.deinit()


if __name__ == "__main__":
    main()
