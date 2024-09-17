import cv2
import numpy as np


# Hàm này lấy tọa độ góc đầu tiên của marker, nếu point bằng 5, lấy tâm
def getMarkerCoordinates(markers, ids, point=0):
    # Số lượng markers = len(markers)
    marker_array = []
    # Tạo danh sách với các góc mong muốn
    for marker in markers:
        marker_array.append(
            [int(marker[0][point][0]), int(marker[0][point][1])])
    # Đặt danh sách theo thứ tự ID của marker

    return marker_array, ids

# Hàm này lấy tọa độ trung tâm của marker từ vật
def getMarkerCenter_foam(marker):
    left_top, ids = getMarkerCoordinates(marker, 1, point=0)  # góc trên bên trái
    right_top, ids = getMarkerCoordinates(marker, 1, point=1)  # góc trên bên phải
    left_bot, ids = getMarkerCoordinates(marker, 1, point=2)  # góc dưới bên trái
    right_bot, ids = getMarkerCoordinates(marker, 1, point=3)  # góc dưới bên phải
    if bool(left_top):
        center_X = (left_top[0][0] + right_top[0][0] + left_bot[0][0] + right_bot[0][0]) * 0.25
        center_Y = (left_top[0][1] + right_top[0][1] + left_bot[0][1] + right_bot[0][1]) * 0.25
        markerCenter = [[int(center_X), int(center_Y)]]
    else:
        markerCenter = [[0, 0]]
    return markerCenter

# Hàm này vẽ các góc lên hình ảnh
def draw_corners(img, corners):
    for corner in corners:
        cv2.circle(img, (corner[0], corner[1]), 10, (0, 255, 0), thickness=-1)

# Hàm này vẽ số ID lên hình ảnh
def draw_numbers(img, corners, ids):
    number = 0
    font = cv2.FONT_HERSHEY_SIMPLEX
    thickness = 4
    for corner in corners:
        cv2.putText(img, str(ids[number]), (corner[0] + 10,
                    corner[1] + 10), font, 1, (0, 0, 0), thickness)
        number += 1


# Hàm này hiển thị thông số lên hình ảnh
def show_spec(img, corners):
    font = cv2.FONT_HERSHEY_SIMPLEX
    thickness = 1
    amountOfCorners = len(corners)
    spec_string = str(amountOfCorners) + " markers found."
    cv2.putText(img, spec_string, (15, 15), font, 0.5, (0, 0, 250), thickness)


def draw_field(img, corners, ids):
    if len(corners) == 4:
        markers_sorted = [0, 0, 0, 0]  # sắp xếp markers
        for sorted_corner_id in [1, 2, 3, 4]:
            index = ids.index(sorted_corner_id)
            markers_sorted[sorted_corner_id - 1] = corners[index]
        contours = np.array(markers_sorted)
        overlay = img.copy()
        cv2.fillPoly(overlay, pts=[contours], color=(255, 215, 0))
        alpha = 0.4  # Hệ số trong suốt
        # Dòng sau đè hình chữ nhật trong suốt lên hình ảnh
        img_new = cv2.addWeighted(overlay, alpha, img, 1 - alpha, 0)
        squarefound = True
    else:
        img_new = img
        squarefound = False
    return img_new, squarefound

# Hàm này sắp xếp các điểm tọa độ
def order_points(pts):
    # Khởi tạo danh sách tọa độ sẽ được sắp xếp sao cho mục đầu tiên trong danh sách là góc trên bên trái,
    # mục thứ hai là góc trên bên phải, mục thứ ba là góc dưới bên phải, và mục thứ tư là góc dưới bên trái
    rect = np.zeros((4, 2), dtype="float32")
    # Điểm góc trên bên trái sẽ có tổng nhỏ nhất, trong khi điểm góc dưới bên phải sẽ có tổng lớn nhất
    s = pts.sum(axis=1)
    rect[0] = pts[np.argmin(s)]
    rect[2] = pts[np.argmax(s)]
    # bây giờ, tính toán sự khác biệt giữa các điểm, điểm góc trên bên phải sẽ có sự khác biệt nhỏ nhất, trong khi điểm góc dưới bên trái sẽ có sự khác biệt lớn nhất
    diff = np.diff(pts, axis=1)
    rect[1] = pts[np.argmin(diff)]
    rect[3] = pts[np.argmax(diff)]
    # trả về tọa độ đã được sắp xếp
    return rect


# Hàm này thực hiện phép biến đổi hình học để có được cái nhìn từ trên xuống của hình ảnh
def four_point_transform(image, pts):
    # Lấy thứ tự nhất quán của các điểm và giải nén chúng
    rect = order_points(pts)
    (tl, tr, br, bl) = rect

    # Tính chiều rộng của hình ảnh mới, sẽ là khoảng cách lớn nhất giữa các điểm dưới bên phải và dưới bên trái theo tọa độ x, hoặc giữa các điểm trên bên phải và trên bên trái theo tọa độ x
    widthA = np.sqrt(((br[0] - bl[0]) ** 2) + ((br[1] - bl[1]) ** 2))
    widthB = np.sqrt(((tr[0] - tl[0]) ** 2) + ((tr[1] - tl[1]) ** 2))
    maxWidth = max(int(widthA), int(widthB))

    # Tính chiều cao của hình ảnh mới, sẽ là khoảng cách lớn nhất giữa các điểm trên bên phải và dưới bên phải theo tọa độ y, hoặc giữa các điểm trên bên trái và dưới bên trái theo tọa độ y
    heightA = np.sqrt(((tr[0] - br[0]) ** 2) + ((tr[1] - br[1]) ** 2))
    heightB = np.sqrt(((tl[0] - bl[0]) ** 2) + ((tl[1] - bl[1]) ** 2))
    maxHeight = max(int(heightA), int(heightB))

    # Bây giờ chúng ta có kích thước của hình ảnh mới, xây dựng bộ điểm đích để có được cái nhìn "từ trên xuống",
    # (nghĩa là cái nhìn từ trên xuống) của hình ảnh, lại chỉ định điểm theo thứ tự trên bên trái, trên bên phải, dưới bên phải, và dưới bên trái
    dst = np.array([
        [0, 0],
        [maxWidth - 1, 0],
        [maxWidth - 1, maxHeight - 1],
        [0, maxHeight - 1]], dtype="float32")

    # Tính toán ma trận biến đổi góc nhìn và sau đó áp dụng nó
    M = cv2.getPerspectiveTransform(rect, dst)
    warped = cv2.warpPerspective(image, M, (maxWidth, maxHeight))

    # Trả về hình ảnh đã được biến đổi
    return warped
