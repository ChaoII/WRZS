//
// Created by AC on 2024-12-24.
//
#include "utils_internal.h"
#include <random>

cv::Mat wimage_to_mat(WImage *image) {
    int cv_type = CV_8UC3;
    if (image->channels == 1) {
        cv_type = CV_8UC1;
    } else if (image->channels == 4) {
        cv_type = CV_8UC4;
    }
    return {image->height, image->width, cv_type, image->data};
}


WImage *mat_to_wimage(const cv::Mat &mat) {
    auto wImage = (WImage *) malloc(sizeof(WImage));
    // 设置宽度和高度
    wImage->width = mat.cols;
    wImage->height = mat.rows;
    wImage->channels = mat.channels();
    wImage->data = (unsigned char *) malloc(mat.total() * mat.elemSize());
    // 复制数据
    std::memcpy(wImage->data, mat.data, mat.total() * mat.elemSize());
    // 设置图像类型
    return wImage;
}

void draw_rect_internal(cv::Mat &cv_image, const cv::Rect &rect, const cv::Scalar &cv_color, double alpha) {
    cv::Mat overlay;
    cv_image.copyTo(overlay);
    cv::rectangle(overlay, {rect.x, rect.y, rect.width, rect.height}, cv_color, -1);
    cv::addWeighted(overlay, alpha, cv_image, 1 - alpha, 0, cv_image);
    cv::rectangle(cv_image, cv::Rect(rect.x, rect.y, rect.width, rect.height), cv_color, 1, cv::LINE_AA, 0);
}

void draw_polygon_internal(cv::Mat &cv_image, const std::vector<cv::Point> &points,
                           const cv::Scalar &color, double alpha) {
    cv::Mat overlay;
    cv_image.copyTo(overlay);
    cv::fillPoly(overlay, points, color, cv::LINE_AA, 0);
    cv::addWeighted(overlay, alpha, cv_image, 1 - alpha, 0, cv_image);
    cv::polylines(cv_image, points, true, color, 1, cv::LINE_AA, 0);
}

void draw_text_internal(cv::Mat &cv_image, const cv::Rect &rect, const std::string &text, const std::string &font_path,
                        int font_size, const cv::Scalar &cv_color, double alpha) {
    draw_rect_internal(cv_image, rect, cv_color, alpha);
    cv::FontFace font(font_path);
    auto size = cv::getTextSize(cv::Size(0, 0), text, cv::Point(rect.x, rect.y), font, font_size);
    cv::rectangle(cv_image, size, cv_color, -1);
    cv::putText(cv_image, text, cv::Point(rect.x, rect.y - 3),
                cv::Scalar(255 - cv_color[0], 255 - cv_color[1], 255 - cv_color[2]), font, font_size);
}

void draw_text_internal(cv::Mat &cv_image, const cv::Point &point, const std::string &text,
                        const std::string &font_path, int font_size, const cv::Scalar &cv_color) {
    cv::FontFace font(font_path);
    cv::putText(cv_image, text, point, cv_color, font, font_size);
}

bool contains_substring(const std::string &str, const std::string &sub_str) {
    return str.find(sub_str) != std::string::npos;
}

std::string format_polygon(WPolygon polygon) {
    std::ostringstream os;
    os << "polygon: {";
    for (int i = 0; i < polygon.size; i++) {
        os << "[" << polygon.data[i].x << "," << polygon.data[i].y << "]";
        if (i != polygon.size - 1) {
            os << ",";
        }
    }
    os << "}";
    return os.str();
}

std::string format_rect(WRect rect) {
    std::ostringstream os;
    os << "WRect {" << "x: " << rect.x << ", " << "y: " << rect.y << ", "
       << "width: " << rect.width << ", " << "height: " << rect.height << "}";
    return os.str();
}

cv::Scalar get_random_color() {
    std::random_device rd;  // 获取随机数种子
    std::mt19937 gen(rd()); // 使用Mersenne Twister算法生成随机数
    std::uniform_int_distribution<> dis(0, 255); // 定义随机数范围为1到255
    return cv::Scalar(dis(gen), dis(gen), dis(gen));
}