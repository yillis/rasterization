import cv2
import glob

if __name__ == "__main__":
    imgs = glob.glob("./img/*.ppm")

    video = cv2.VideoWriter('video.avi', cv2.VideoWriter_fourcc('X', 'V', 'I', 'D'), 60, (1000, 1000))

    for img_name in imgs:
        img = cv2.imread(img_name)
        video.write(img)
    
    video.release()
