import cv2
import pygame
import data


def play_video(random_number):
    video_folder = "./videos"
    video_name = data.folder_mapping.get(random_number)
    audio_name = video_name + '_audio'
    pygame.init()

    # Initialize Pygame display (not used for video playback)
    screen = pygame.display.set_mode((640, 480))
    pygame.display.set_caption("Video with Audio")

    # Load the video file
    cap = cv2.VideoCapture(video_folder + '/' + video_name + '.mp4')

    # Load the audio
    pygame.mixer.init()
    pygame.mixer.music.load(video_folder + '/' + audio_name + '.mp3')
    pygame.mixer.music.play()

    while cap.isOpened():
        ret, frame = cap.read()
        if not ret:
            break
        cv2.imshow("Video", frame)
        if cv2.waitKey(30) & 0xFF == ord('q'):
            break
    data.additional_column_value = random_number

    cap.release()
    cv2.destroyAllWindows()
    pygame.mixer.quit()
    pygame.quit()
