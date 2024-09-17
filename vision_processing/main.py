import threading
import numpy as np
import cv2
import time
from hc06 import BluetoothSocketClient
from ArucoDetection import (
    draw_corners,
    draw_numbers,
    draw_field,
    four_point_transform,
    getMarkerCenter_foam,
    getMarkerCoordinates,
    show_spec,
)

# Thời gian bắt đầu
start_time = time.time()

# Khởi tạo biến toàn cục
centerCorner = [[0, 0]]
h, w = 0, 0


# Hàm này lấy các marker từ khung hình video
def get_markers(vid_frame, aruco_dictionary, aruco_parameters):
    bboxs, ids, rejected = cv2.aruco.ArucoDetector(aruco_dictionary, aruco_parameters).detectMarkers(vid_frame)
    ids_sorted = [id[0] for id in ids] if ids is not None else ids
    return bboxs, ids_sorted


current_square_points = [[0, 30], [30, 30], [30, 0], [0, 0]]
current_center_Corner = [[0, 0]]

# Sử dụng giữ vị trí
marker_location_hold = True


def send_data(client):
    global centerCorner, h, w
    prev_data_to_send = None

    while True:
        if h == 0 or w == 0:
            time.sleep(5)
            continue

        x_coordinate = int((centerCorner[0][1] / h) * 30) - 15
        y_coordinate = int((centerCorner[0][0] / w) * 15)

        data_to_send = (x_coordinate, y_coordinate)

        if data_to_send == prev_data_to_send:
            time.sleep(5)
            continue

        client.send_message(str(data_to_send).encode())
        print(f"Sent: {x_coordinate}; {y_coordinate}")
        # Cập nhật giá trị trước đó
        prev_data_to_send = data_to_send
        time.sleep(5)


def main():
    global centerCorner, h, w

    try:
        client = BluetoothSocketClient('00:23:07:36:24:EC', 1)
        client.connect()
    except Exception as e:
        print(f"Error connecting to Bluetooth: {e}")
        return

    # Tải từ điển ArUco
    print("Đang phát hiện markers '{}'...".format("DICT_4X4_50"))
    aruco_dict1 = cv2.aruco.getPredefinedDictionary(cv2.aruco.DICT_4X4_50)
    aruco_param1 = cv2.aruco.DetectorParameters()
    aruco_dict2 = cv2.aruco.getPredefinedDictionary(cv2.aruco.DICT_6X6_50)
    aruco_param2 = cv2.aruco.DetectorParameters()

    # Bắt đầu luồng video
    url = 'http://172.20.10.2:4747/video'
    cap = cv2.VideoCapture(url)

    # Khởi tạo luồng gửi dữ liệu
    send_thread = threading.Thread(target=send_data, args=(client,))
    send_thread.start()

    while True:
        current_time = time.time()
        ret, frame = cap.read()

        markers, ids = get_markers(frame, aruco_dict1, aruco_param1)
        frame_clean = frame.copy()
        left_corners, corner_ids = getMarkerCoordinates(markers, ids, 0)

        if marker_location_hold:
            if corner_ids is not None:
                count = 0
                for id in corner_ids:
                    if id > 4:  # Đảm bảo chỉ sử dụng các ids từ 5 đến 8
                        break
                    current_square_points[id - 1] = left_corners[count]
                    count += 1
            left_corners = current_square_points
            corner_ids = [1, 2, 3, 4]

        if start_time < current_time:
            cv2.aruco.drawDetectedMarkers(frame, markers)
            draw_corners(frame, left_corners)
            draw_numbers(frame, left_corners, corner_ids)
            show_spec(frame, left_corners)

        frame_with_square, squareFound = draw_field(frame, left_corners, corner_ids)

        if start_time < current_time:
            if squareFound:
                square_points = left_corners
            img_wrapped = four_point_transform(frame_clean, np.array(square_points))
            h, w, c = img_wrapped.shape
            marker_foam, ids_foam = get_markers(img_wrapped, aruco_dict2, aruco_param2)
            left_corner_foam, corner_id_foam = getMarkerCoordinates(marker_foam, ids_foam, 0)
            centerCorner = getMarkerCenter_foam(marker_foam)

            if marker_location_hold:
                if corner_id_foam is not None:
                    current_center_Corner[0] = centerCorner[0]
                centerCorner[0] = current_center_Corner[0]

            draw_corners(img_wrapped, centerCorner)
            img_wrapped = cv2.line(img_wrapped, (centerCorner[0][0], 0), (centerCorner[0][0], h), (0, 0, 255), 1)
            img_wrapped = cv2.line(img_wrapped, (0, (centerCorner[0][1])), (w, (centerCorner[0][1])), (0, 0, 255), 1)

            draw_numbers(img_wrapped, left_corner_foam, corner_id_foam)
            cv2.imshow('img_wrapped', img_wrapped)

        cv2.imshow('frame_with_square', frame_with_square)

        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    client.close()
    cap.release()
    cv2.destroyAllWindows()
    return centerCorner


if __name__ == "__main__":
    main()
