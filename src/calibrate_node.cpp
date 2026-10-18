#include<opencv2/opencv.hpp>
#include<string>
#include<iostream>
#include<cmath>
#include<vector>

using namespace std;
using namespace cv;


    Size pattern_size_;
    double square_size_;
    bool corners_found_ = false;
    vector<Point2f>last_corners_;
    vector<vector<Point2f>> image_points_;

class CalibrateNode{
    public:CalibrateNode(){
        pattern_size_ = Size(11,8);
        square_size_ = 0.05f;}


    private:
    
    bool process_images(Mat & images)
    {
        Mat gray;
        cvtColor(images,gray,COLOR_BGR2GRAY);

        vector<Point2f>corners;
        bool found = findChessboardCorners(gray,pattern_size_,corners);

        if (found) {
            cornerSubPix(gray, corners, Size(11, 11), Size(-1, -1),
                TermCriteria(TermCriteria::EPS + TermCriteria::MAX_ITER, 30, 0.001));
            drawChessboardCorners(images, pattern_size_, corners, found);
            last_corners_ = corners;
            corners_found_ = true;
        } else {
            corners_found_ = false;
        }

        imshow("Calibration", images);
        waitKey(1);
        }

        void run_calibration()
        {
            vector<vector<Point3f>>object_points;
            vector<Point3f>obj;

            for (int i = 0; i < pattern_size_.height; i++)
                for (int j = 0; j < pattern_size_.width; j++)
                    obj.push_back(Point3f(j * square_size_, i * square_size_, 0));
            for (size_t i = 0; i < image_points_.size(); i++)
                object_points.push_back(obj);

            Mat camera_matrix, dist_coeffs;
            vector<Mat> rvecs, tvecs;
            double rms = calibrateCamera(object_points, image_points_,
                Size(640, 480), camera_matrix, dist_coeffs, rvecs, tvecs);



        
            FileStorage fs("/home/yaoduoduo/my_calibration.yaml", FileStorage::WRITE);
            if (!fs.isOpened()) {
                cerr << "无法写入标定文件" << endl;
                return;
            }
            fs << "camera_matrix" << camera_matrix;
            fs << "distortion_coefficients" << dist_coeffs;
            fs.release();
            cout << "标定结果已保存到 /home/yaoduoduo/my_calibration.yaml" << endl;
    
        }

        
    };

        
int main(){
        string pattern = "/home/yaoduoduo/Downloads/calibration_images/image_000001.png";  // 可改为.png、.bmp 等
        vector<String> filenames;
        glob(pattern, filenames, false);  

        if (filenames.empty()) {
        cout << "没有找到匹配的图片文件" << endl;
        exit(1);
    }



        vector<Mat>images;
        for(const auto & name : filenames ){
            Mat img = imread(name,IMREAD_COLOR);
            if (img.empty()) {
            cerr << "读取失败: " << name << endl;
            continue;  
        }
        images.push_back(img);
    }

    
   CalibrateNode node;
   
    
    destroyAllWindows();

    return 0;
        
        
    };

