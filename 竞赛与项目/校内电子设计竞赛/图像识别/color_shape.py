import time, os, sys
from media.sensor import *
from media.display import *
from media.media import *
import image

# 设置图像捕获分辨率 / Set picture capture resolution
PICTURE_WIDTH = 400
PICTURE_HEIGHT = 240

# 显示模式
DISPLAY_MODE = "LCD"

# 根据显示模式设置分辨率
if DISPLAY_MODE == "VIRT":
    DISPLAY_WIDTH = ALIGN_UP(1920, 16)
    DISPLAY_HEIGHT = 1080
elif DISPLAY_MODE == "LCD":
    DISPLAY_WIDTH = 640
    DISPLAY_HEIGHT = 480
else:
    raise ValueError("未知的显示模式")

# ====================== 颜色识别配置（你提供的稳定模块） ======================
THRESHOLDS = [
    (0, 66, 7, 127, 3, 127),      # 红
    (42, 100, -128, -17, 6, 66),  # 绿
    (43, 99, -43, -4, -56, -7),   # 蓝
]
COLOR_NAMES = ["RED", "GREEN", "BLUE"]

def get_closest_rgb(lab_threshold):
    l_center = (lab_threshold[0] + lab_threshold[1]) // 2
    a_center = (lab_threshold[2] + lab_threshold[3]) // 2
    b_center = (lab_threshold[4] + lab_threshold[5]) // 2
    return image.lab_to_rgb((l_center, a_center, b_center))

def detect_color(img):
    best_color = -1
    best_blob = None
    for i in range(3):
        blobs = img.find_blobs([THRESHOLDS[i]], area_threshold=200, merge=True, margin=10)
        if blobs:
            max_b = max(blobs, key=lambda b: b.area())
            if best_blob is None or max_b.area() > best_blob.area():
                best_blob = max_b
                best_color = i
    if best_blob is not None and best_color != -1:
        color_rgb = get_closest_rgb(THRESHOLDS[best_color])
        img.draw_rectangle(best_blob.rect(), color=color_rgb, thickness=2)
        return COLOR_NAMES[best_color]
    return "NONE"

# ====================== 初始化（完全不变） ======================
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

# ====================== 原有圆形识别（完全不动） ======================
def process_circles(img, circles):
    print("【圆形信息】")
    for i, circle in enumerate(circles):
        img.draw_circle(circle.circle(), color=(40,167,225), thickness=3)
        print(f"Circle {i}: {circle}")
    print("========================")

# ====================== 矩形识别（使用你提供的 find_rects 方法） ======================
def process_rects(img):
    print("【矩形信息】")
    rect_count = 0
    # 完全使用你给的代码写法！
    for r in img.find_rects(threshold=8000):
        img.draw_rectangle(r.rect(), color=(40, 167, 225), thickness=2)
        for p in r.corners():
            img.draw_circle(p[0], p[1], 8, color=(78, 90, 34))
        print(r)
        rect_count += 1
    print("========================")
    return rect_count

# ====================== 主函数 ======================
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

            # 1. 圆形识别（原来的）
            circles = img.find_circles(threshold=3500)
            process_circles(img, circles)

            # 2. 矩形识别（你提供的 find_rects 方法）
            rect_num = process_rects(img)

            # ====================== 【新增：颜色识别，不改动原代码】 ======================
            detected_color = detect_color(img)

            # 判断图形名称
            if len(circles) > 0:
                shape_name = "CIRCLE"
            elif rect_num > 0:
                shape_name = "RECTANGLE"
            else:
                shape_name = "NO SHAPE"

            # ====================== 【屏幕显示结果】 ======================
            img.draw_string(10, 10, f"SHAPE: {shape_name}", color=(255,255,0), scale=2)
            img.draw_string(10, 30, f"COLOR: {detected_color}", color=(0,255,255), scale=2)

            # 显示图像
            Display.show_image(img, x=x_offset, y=y_offset)

    except KeyboardInterrupt:
        print("用户中断")
    except Exception as e:
        print(f"错误: {e}")
    finally:
        if 'sensor' in locals() and isinstance(sensor, Sensor):
            sensor.stop()
        Display.deinit()
        os.exitpoint(os.EXITPOINT_ENABLE_SLEEP)
        time.sleep_ms(100)
        MediaManager.deinit()

if __name__ == "__main__":
    main()
