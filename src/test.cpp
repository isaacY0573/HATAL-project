#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

// Function prototypes
void rotateVideo(Mat &frame, int rotationAngle);
void resizeVideo(Mat &frame, int newWidth, int newHeight);
void applyFilter(Mat &frame, int filterOption);
void addText(Mat &frame, const string &userText);
void trimVideo(VideoCapture &cap, int &startFrame, int &endFrame, double fps, int totalFrames);

// Function to handle video processing
// Function to handle video processing
void processVideo(const string &videoFilePath, const string &outputFilePath, int startFrame, int endFrame, 
                  int rotationAngle, int newWidth, int newHeight, int filterOption, const string &userText, double fps) {
    // Open the video file using OpenCV's VideoCapture
    VideoCapture cap(videoFilePath);

    // Check if the video file was opened successfully
    if (!cap.isOpened()) {
        cerr << "Error: Could not open video file." << endl;
        return;
    }

    int originalWidth = cap.get(CAP_PROP_FRAME_WIDTH);
    int originalHeight = cap.get(CAP_PROP_FRAME_HEIGHT);

    // If the user entered 0 for width or height, use the original size
    if (newWidth == 0 || newHeight == 0) {
        newWidth = originalWidth;
        newHeight = originalHeight;
    }

    Mat frame;

    // Read the first frame to determine new dimensions after rotation
    if (!cap.read(frame)) {
        cerr << "Error: Could not read the first frame." << endl;
        return;
    }

    // Apply rotation and resizing to the first frame
    rotateVideo(frame, rotationAngle);
    resizeVideo(frame, newWidth, newHeight);

    // Get new dimensions after rotation
    int rotatedWidth = frame.cols;
    int rotatedHeight = frame.rows;

    // Debug: Print the new dimensions
    cout << "Rotated Width: " << rotatedWidth << ", Rotated Height: " << rotatedHeight << endl;

    // Use a different codec for VideoWriter (Try XVID or MP4V)
    VideoWriter writer(outputFilePath, VideoWriter::fourcc('X', 'V', 'I', 'D'), fps, Size(rotatedWidth, rotatedHeight));

    // Check if the VideoWriter was initialized successfully
    if (!writer.isOpened()) {
        cerr << "Error: Could not create video writer. Check codec and output file path." << endl;
        return;
    }

    int currentFrame = startFrame; // Start from the user-specified frame

    // Rewind the video back to the start frame
    cap.set(CAP_PROP_POS_FRAMES, startFrame);

    // Read frames from the video
    while (currentFrame < endFrame) {
        bool isSuccess = cap.read(frame);
        if (!isSuccess || currentFrame >= endFrame) {
            cout << "End of video." << endl;
            break;
        }

        // Apply the processing functions
        resizeVideo(frame, newWidth, newHeight);
        rotateVideo(frame, rotationAngle);
        applyFilter(frame, filterOption);
        addText(frame, userText);

        // Write the processed frame to the output video
        if (frame.empty()) {
            cerr << "Warning: The frame is empty at frame number " << currentFrame << endl;
        } else {
            writer.write(frame);
        }

        // Display the frame in the window
        imshow("Video", frame);

        // Wait for 30 ms before moving to the next frame
        if (waitKey(30) == 'q') {
            cout << "Video playback interrupted by user." << endl;
            break;
        }

        currentFrame++;
    }

    // Release the video capture and writer objects
    cap.release();
    writer.release();
    destroyAllWindows();
    cout << "Processed video saved to: " << outputFilePath << endl;
}




// Function to handle resizing
void resizeVideo(Mat &frame, int newWidth, int newHeight) {
    if (newWidth > 0 && newHeight > 0) {
        resize(frame, frame, Size(newWidth, newHeight));
    }
}

// Function to handle rotation
void rotateVideo(Mat &frame, int rotationAngle) {
    if (rotationAngle == 90) {
        rotate(frame, frame, ROTATE_90_CLOCKWISE);
    } else if (rotationAngle == 180) {
        rotate(frame, frame, ROTATE_180);
    } else if (rotationAngle == 270) {
        rotate(frame, frame, ROTATE_90_COUNTERCLOCKWISE);
    }
}

// Function to apply filters
void applyFilter(Mat &frame, int filterOption) {
    if (filterOption == 1) {
        // Convert to grayscale
        cvtColor(frame, frame, COLOR_BGR2GRAY);
        cvtColor(frame, frame, COLOR_GRAY2BGR); // Convert back to BGR for compatibility with VideoWriter
    } else if (filterOption == 2) {
        // Apply blur filter
        GaussianBlur(frame, frame, Size(15, 15), 0);
    }
}

// Function to add text to the frame
void addText(Mat &frame, const string &userText) {
    int fontFace = FONT_HERSHEY_SIMPLEX;
    double fontScale = 1;
    int thickness = 2;
    Scalar color(255, 0, 0); // Blue color in BGR
    Point textOrg(30, 50);   // Position the text
    putText(frame, userText, textOrg, fontFace, fontScale, color, thickness);
}

// Function to handle trimming
void trimVideo(VideoCapture &cap, int &startFrame, int &endFrame, double fps, int totalFrames) {
    double startTime, endTime;
    cout << "Enter the start time (in seconds): ";
    cin >> startTime;
    cout << "Enter the end time (in seconds): ";
    cin >> endTime;

    // Calculate the frame numbers corresponding to start and end times
    startFrame = static_cast<int>(startTime * fps);
    endFrame = static_cast<int>(endTime * fps);

    // Ensure frame numbers are within bounds
    if (startFrame < 0 || endFrame >= totalFrames || startFrame >= endFrame) {
        cerr << "Error: Invalid start or end time." << endl;
        startFrame = 0;
        endFrame = totalFrames;
    }
}

// Main function
int main() {
    string videoFilePath, outputFilePath, userText;
    double fps;
    int newWidth = 0, newHeight = 0, rotationAngle = 0, filterOption = 0;
    int startFrame = 0, endFrame = 0;

    // Prompt the user for video file path and other details
    cout << "Enter the video file path: ";
    getline(cin, videoFilePath);
    cout << "Enter the output file path (including .avi extension): ";
    getline(cin, outputFilePath);

    // Open the video file using OpenCV's VideoCapture to get video properties
    VideoCapture cap(videoFilePath);
    if (!cap.isOpened()) {
        cerr << "Error: Could not open video file." << endl;
        return 1;
    }
    fps = cap.get(CAP_PROP_FPS);
    int totalFrames = cap.get(CAP_PROP_FRAME_COUNT);

    // Present a menu to the user for each processing option
    int choice;
    cout << "\nChoose video processing options:\n";
    cout << "1. Trim Video\n2. Rotate Video\n3. Resize Video\n4. Apply Filter\n5. Add Text\n";
    cout << "Enter your choice (press 0 to skip): ";
    cin >> choice;

    // Based on user choice, prompt for details of the selected option
    if (choice == 1) {
        // Trimming video
        trimVideo(cap, startFrame, endFrame, fps, totalFrames);
    } else {
        startFrame = 0;
        endFrame = totalFrames;
    }
    if (choice == 2) {
        cout << "Enter rotation angle (0, 90, 180, 270): ";
        cin >> rotationAngle;
    }
    if (choice == 3) {
        cout << "Enter new width for the video (or 0 to keep original size): ";
        cin >> newWidth;
        cout << "Enter new height for the video (or 0 to keep original size): ";
        cin >> newHeight;
    }
    if (choice == 4) {
        cout << "Select a filter to apply: \n";
        cout << "1 - Grayscale\n2 - Blur\n";
        cout << "Enter your choice: ";
        cin >> filterOption;
    }
    if (choice == 5) {
        cout << "Enter the text to display on the video: ";
        cin.ignore(); // Clear the input buffer
        getline(cin, userText);
    }

    // Call the processing function with the user's inputs
    processVideo(videoFilePath, outputFilePath, startFrame, endFrame, rotationAngle, newWidth, newHeight, filterOption, userText, fps);

    return 0;
}
















// #include <iostream>
// #include <string>
// #include <opencv2/opencv.hpp>

// using namespace std;
// using namespace cv;

// // Function prototypes
// void rotateVideo(Mat &frame, int rotationAngle);
// void resizeVideo(Mat &frame, int newWidth, int newHeight);
// void applyFilter(Mat &frame, int filterOption);
// void addText(Mat &frame, const string &userText);
// void trimVideo(VideoCapture &cap, int &startFrame, int &endFrame, double fps, int totalFrames);

// // Function to handle video processing
// // Function to handle video processing
// void processVideo(const string &videoFilePath, const string &outputFilePath, int startFrame, int endFrame, 
//                   int rotationAngle, int newWidth, int newHeight, int filterOption, const string &userText, double fps) {
//     // Open the video file using OpenCV's VideoCapture
//     VideoCapture cap(videoFilePath);

//     // Check if the video file was opened successfully
//     if (!cap.isOpened()) {
//         cerr << "Error: Could not open video file." << endl;
//         return;
//     }

//     int originalWidth = cap.get(CAP_PROP_FRAME_WIDTH);
//     int originalHeight = cap.get(CAP_PROP_FRAME_HEIGHT);

//     // If the user entered 0 for width or height, use the original size
//     if (newWidth == 0 || newHeight == 0) {
//         newWidth = originalWidth;
//         newHeight = originalHeight;
//     }

//     Mat frame;

//     // Read the first frame to determine new dimensions after rotation
//     if (!cap.read(frame)) {
//         cerr << "Error: Could not read the first frame." << endl;
//         return;
//     }
//     // Rotate and resize the first frame to determine new dimensions
//     rotateVideo(frame, rotationAngle);
//     resizeVideo(frame, newWidth, newHeight);

//     // Create a VideoWriter with updated dimensions after possible rotation
//     VideoWriter writer(outputFilePath, VideoWriter::fourcc('M', 'J', 'P', 'G'), fps, Size(newWidth, newHeight));

//     // Check if the VideoWriter was initialized successfully
//     if (!writer.isOpened()) {
//         cerr << "Error: Could not create video writer." << endl;
//         return;
//     }

//     int currentFrame = startFrame; // Start from the user-specified frame

//     // Rewind the video back to the start frame
//     cap.set(CAP_PROP_POS_FRAMES, startFrame);

//     // Read frames from the video
//     while (currentFrame < endFrame) {
//         bool isSuccess = cap.read(frame);
//         if (!isSuccess || currentFrame >= endFrame) {
//             cout << "End of video." << endl;
//             break;
//         }

//         // Apply the processing functions
//         resizeVideo(frame, newWidth, newHeight);
//         rotateVideo(frame, rotationAngle);
//         applyFilter(frame, filterOption);
//         addText(frame, userText);

//         // Write the processed frame to the output video
//         if (frame.empty()) {
//     cerr << "Warning: The frame is empty at frame number " << currentFrame << endl;
// }   else {
//     writer.write(frame);
// }


//         // Display the frame in the window
//         imshow("Video", frame);

//         // Wait for 30 ms before moving to the next frame
//         if (waitKey(30) == 'q') {
//             cout << "Video playback interrupted by user." << endl;
//             break;
//         }

//         currentFrame++;
//     }

//     // Release the video capture and writer objects
//     cap.release();
//     writer.release();
//     destroyAllWindows();
//     cout << "Processed video saved to: " << outputFilePath << endl;
// }


// // Function to handle resizing
// void resizeVideo(Mat &frame, int newWidth, int newHeight) {
//     if (newWidth > 0 && newHeight > 0) {
//         resize(frame, frame, Size(newWidth, newHeight));
//     }
// }

// // Function to handle rotation
// void rotateVideo(Mat &frame, int rotationAngle) {
//     if (rotationAngle == 90) {
//         rotate(frame, frame, ROTATE_90_CLOCKWISE);
//     } else if (rotationAngle == 180) {
//         rotate(frame, frame, ROTATE_180);
//     } else if (rotationAngle == 270) {
//         rotate(frame, frame, ROTATE_90_COUNTERCLOCKWISE);
//     }
// }

// // Function to apply filters
// void applyFilter(Mat &frame, int filterOption) {
//     if (filterOption == 1) {
//         // Convert to grayscale
//         cvtColor(frame, frame, COLOR_BGR2GRAY);
//         cvtColor(frame, frame, COLOR_GRAY2BGR); // Convert back to BGR for compatibility with VideoWriter
//     } else if (filterOption == 2) {
//         // Apply blur filter
//         GaussianBlur(frame, frame, Size(15, 15), 0);
//     }
// }

// // Function to add text to the frame
// void addText(Mat &frame, const string &userText) {
//     int fontFace = FONT_HERSHEY_SIMPLEX;
//     double fontScale = 1;
//     int thickness = 2;
//     Scalar color(255, 0, 0); // Blue color in BGR
//     Point textOrg(30, 50);   // Position the text
//     putText(frame, userText, textOrg, fontFace, fontScale, color, thickness);
// }

// // Function to handle trimming
// void trimVideo(VideoCapture &cap, int &startFrame, int &endFrame, double fps, int totalFrames) {
//     double startTime, endTime;
//     cout << "Enter the start time (in seconds): ";
//     cin >> startTime;
//     cout << "Enter the end time (in seconds): ";
//     cin >> endTime;

//     // Calculate the frame numbers corresponding to start and end times
//     startFrame = static_cast<int>(startTime * fps);
//     endFrame = static_cast<int>(endTime * fps);

//     // Ensure frame numbers are within bounds
//     if (startFrame < 0 || endFrame >= totalFrames || startFrame >= endFrame) {
//         cerr << "Error: Invalid start or end time." << endl;
//         startFrame = 0;
//         endFrame = totalFrames;
//     }
// }

// // Main function
// int main() {
//     string videoFilePath, outputFilePath, userText;
//     double fps;
//     int newWidth = 0, newHeight = 0, rotationAngle = 0, filterOption = 0;
//     int startFrame = 0, endFrame = 0;

//     // Prompt the user for video file path and other details
//     cout << "Enter the video file path: ";
//     getline(cin, videoFilePath);
//     cout << "Enter the output file path (including .avi extension): ";
//     getline(cin, outputFilePath);

//     // Open the video file using OpenCV's VideoCapture to get video properties
//     VideoCapture cap(videoFilePath);
//     if (!cap.isOpened()) {
//         cerr << "Error: Could not open video file." << endl;
//         return 1;
//     }
//     fps = cap.get(CAP_PROP_FPS);
//     int totalFrames = cap.get(CAP_PROP_FRAME_COUNT);

//     // Present a menu to the user for each processing option
//     int choice;
//     cout << "\nChoose video processing options:\n";
//     cout << "1. Trim Video\n2. Rotate Video\n3. Resize Video\n4. Apply Filter\n5. Add Text\n";
//     cout << "Enter your choice (press 0 to skip): ";
//     cin >> choice;

//     // Based on user choice, prompt for details of the selected option
//     if (choice == 1) {
//         // Trimming video
//         trimVideo(cap, startFrame, endFrame, fps, totalFrames);
//     } else {
//         startFrame = 0;
//         endFrame = totalFrames;
//     }
//     if (choice == 2) {
//         cout << "Enter rotation angle (0, 90, 180, 270): ";
//         cin >> rotationAngle;
//     }
//     if (choice == 3) {
//         cout << "Enter new width for the video (or 0 to keep original size): ";
//         cin >> newWidth;
//         cout << "Enter new height for the video (or 0 to keep original size): ";
//         cin >> newHeight;
//     }
//     if (choice == 4) {
//         cout << "Select a filter to apply: \n";
//         cout << "1 - Grayscale\n2 - Blur\n";
//         cout << "Enter your choice: ";
//         cin >> filterOption;
//     }
//     if (choice == 5) {
//         cout << "Enter the text to display on the video: ";
//         cin.ignore(); // Clear the input buffer
//         getline(cin, userText);
//     }

//     // Call the processing function with the user's inputs
//     processVideo(videoFilePath, outputFilePath, startFrame, endFrame, rotationAngle, newWidth, newHeight, filterOption, userText, fps);

//     return 0;
// }

