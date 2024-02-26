#include "Image.h"
#include <string>
#include <set>
#include <opencv2/imgproc.hpp>
#include <math.h>


cv::Mat newref;

std::string get_file_extension(const std::string& filepath) {
    size_t dot_pos = filepath.find_last_of('.');

    if (dot_pos == std::string::npos) {
        return ""; // Empty extension
    }

    return filepath.substr(dot_pos + 1);
}

std::string get_filename(const std::string& filepath) {
    size_t separator_pos = filepath.find_last_of('/');

    if (separator_pos == std::string::npos) {
        separator_pos = filepath.find_last_of('\\');
    }

    if (separator_pos == std::string::npos) {
        return ""; // Empty filename
    }

    return filepath.substr(separator_pos + 1);
}

GLuint get_texturID(cv::Mat image, GLenum minFilter = GL_LINEAR_MIPMAP_LINEAR, GLenum magFilter = GL_LINEAR, GLenum wrapFilter = GL_CLAMP) {
        GLuint textureID;
        glGenTextures(1, &textureID);

        // Bind to our texture handle
        glBindTexture(GL_TEXTURE_2D, textureID);

        // Catch silly-mistake texture interpolation method for magnification
        if (magFilter == GL_LINEAR_MIPMAP_LINEAR ||
            magFilter == GL_LINEAR_MIPMAP_NEAREST ||
            magFilter == GL_NEAREST_MIPMAP_LINEAR ||
            magFilter == GL_NEAREST_MIPMAP_NEAREST)
        {
            std::cout << "You can't use MIPMAPs for magnification - setting filter to GL_LINEAR" << std::endl;
            magFilter = GL_LINEAR;
        }

        // use fast 4-byte alignment (default anyway) if possible
        glPixelStorei(GL_UNPACK_ALIGNMENT, (image.step & 3) ? 1 : 4);

        //set length of one complete row in data (doesn't need to equal image.cols)
        glPixelStorei(GL_UNPACK_ROW_LENGTH, image.step / image.elemSize());

        // Set texture interpolation methods for minification and magnification
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

        // Set texture clamping method
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapFilter);

        // Set incoming texture format to:
        // GL_BGR       for CV_CAP_OPENNI_BGR_IMAGE,
        // GL_LUMINANCE for CV_CAP_OPENNI_DISPARITY_MAP,
        // Work out other mappings as required ( there's a list in comments in main() )
        GLenum inputColourFormat = GL_BGR;
        if (image.channels() == 1) inputColourFormat = GL_LUMINANCE;
        else if (image.channels() == 4) inputColourFormat = GL_BGRA;

        // Create the texture
        glTexImage2D(GL_TEXTURE_2D,     // Type of texture
            0,                 // Pyramid level (for mip-mapping) - 0 is the top level
            GL_RGB,            // Internal colour format to convert to
            image.cols,          // Image width  i.e. 640 for Kinect in standard mode
            image.rows,          // Image height i.e. 480 for Kinect in standard mode
            0,                 // Border width in pixels (can either be 1 or 0)
            inputColourFormat, // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
            GL_UNSIGNED_BYTE,  // Image data type
            image.ptr());        // The actual image data itself

        // If we're using mipmaps then generate them. Note: This requires OpenGL 3.0 or higher
        if (minFilter == GL_LINEAR_MIPMAP_LINEAR ||
            minFilter == GL_LINEAR_MIPMAP_NEAREST ||
            minFilter == GL_NEAREST_MIPMAP_LINEAR ||
            minFilter == GL_NEAREST_MIPMAP_NEAREST)
        {
            glGenerateMipmap(GL_TEXTURE_2D);
        }

        return textureID;
}

Image::Image(std::string filepath) {

    std::string extension = get_file_extension(filepath);

    std::set<std::string> valid_extensions = { "jpg", "jpeg", "png" };

    if (valid_extensions.find(extension) != valid_extensions.end()) {
        // Matched a valid extension
        if (read(filepath)) {
            std::cout << "Image Failed to Load" << std::endl;
        }
        else {
            std::cout << "Image Loaded" << std::endl;
            width = Img.cols;
            height = Img.rows;
            channels = Img.channels();
            pixels = Img.ptr();
            filename = get_filename(filepath);
            texID = get_texturID(Img);
            orgTexID = texID;
            copy = Img.clone();
            newref = copy;


        }
    }
    else {
        // No match found
        std::cout << "Not a valid file" << std::endl;
    }
    
}

Image::Image(int w, int h, int ch){
}

Image::Image(const Image& img){
    Img = img.Img;
}

Image::~Image(){}

bool Image::read(std::string filename){
    Img = cv::imread(filename);
    return Img.empty();
}

int Image::cv_display(std::string name) {
    cv::namedWindow(name, cv::WINDOW_NORMAL);
    cv::imshow(name, Img);
    return cv::waitKey(2);
}

//void Image::ImGui_display()
//{
//    // Get the available space within the ImGui window
//    ImVec2 imageSize = ImGui::GetContentRegionAvail();
//
//    // Display image using the OpenGL texture ID
//    ImGui::Begin("Image", (bool*)true, ImGuiWindowFlags_NoTitleBar);
//
//    // Use the texture ID for the image
//    ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(texID)), imageSize);
//    std::cout << "image showingg in imgui window";
//    std::cout << "texture : " << texID;
//
//
//    ImGui::End();
//}

bool Image::write(std::string filename){
    copy = newref;
    return cv::imwrite(filename, copy);
}

void Image::grayscale_avg()
{
    cv::Mat image = Img.clone();
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            cv::Vec3b pixel = image.at<cv::Vec3b>(y, x);
            unsigned char gray = (pixel[0] + pixel[1] + pixel[2]) / 3;
            image.at<cv::Vec3b>(y, x) = cv::Vec3b(gray, gray, gray);
        }
    }
    texID = get_texturID(image);
}

void Image::grayscale_lum()
{
    cv::Mat image = Img.clone();
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            cv::Vec3b pixel = image.at<cv::Vec3b>(y, x);
            unsigned char gray = 0.0722*pixel[0] + 0.7152*pixel[1] + 0.2126*pixel[2];
            image.at<cv::Vec3b>(y, x) = cv::Vec3b(gray, gray, gray);
        }
    }
    texID = get_texturID(image);
}

void Image::color_mask(float* r, float* g, float* b)
{
    if (lastedited != "mask")copy = newref;

    cv::Mat image = copy.clone();
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            cv::Vec3b pixel = image.at<cv::Vec3b>(y, x);
            pixel[0] *= *b;
            pixel[1] *= *g;
            pixel[2] *= *r;

            image.at<cv::Vec3b>(y, x) = pixel;
        }
    }
    newref = image;
    texID = get_texturID(image);
}

void Image::brightness(float* bright)
{

    //clone the original image
    if (lastedited != "bright")copy = newref;
    cv::Mat image = copy.clone();

    //incraese the brightness
    image = image + cv::Scalar(*bright, *bright, *bright);
    
    //if (*bright != 1) newref = image;
    //get texture id of changed image
    newref = image;
    texID = get_texturID(image);
}

void Image::contrast(float* contrast)
{
    if (lastedited != "contrast")copy = newref;
    cv::Mat image = copy.clone();
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            // Iterate through each channel (assuming 3 channels BGR)
            for (int c = 0; c < image.channels(); ++c) {
                double pixelValue = image.at<cv::Vec3b>(y, x)[c];
                double newPixelValue = *contrast * (pixelValue - 128) + 128;
                image.at<cv::Vec3b>(y, x)[c] = cv::saturate_cast<uchar>(newPixelValue);
            }
        }
    }
    newref = image;
    texID = get_texturID(image);
}

void Image::hue_saturation(int* deltahue, float* sat)
{
    if (lastedited != "hue_sat")copy = newref;
    cv::Mat image = copy.clone();
    double new_sat = *sat * 2.2;

    // Convert BGR image to HSV
    cv::Mat hsvImage;
    cv::cvtColor(image, hsvImage, cv::COLOR_BGR2HSV_FULL);

    for (int j = 0; j < height; ++j) {
        for (int i = 0; i < width; ++i) {
            //double  hue = hsvImage.at<cv::Vec3b>(j, i)[0];
            uchar  saturate = hsvImage.at<cv::Vec3b>(j, i)[1];

            /*uchar h_plus_shift = hue;
            h_plus_shift += *deltahue;
            if (h_plus_shift < 0)
                hue = 180 + h_plus_shift;
            else if (h_plus_shift > 180)
                hue = h_plus_shift - 180;
            else
                hue = h_plus_shift;

            hsvImage.at<cv::Vec3b>(j, i)[0] = hue;*/

            
            uchar s_plus_shift = saturate +  new_sat;

            if (s_plus_shift < 0)
                s_plus_shift = 0;
            else if (s_plus_shift > 255)
                s_plus_shift = 255;

            hsvImage.at<cv::Vec3b>(j, i)[1] = s_plus_shift;

        }
    }
    // Convert back to BGR
    cv::cvtColor(hsvImage, image, cv::COLOR_HSV2BGR_FULL);
    newref = image;
    texID = get_texturID(image);
}

void Image::gammaCorrection( float* gamma) {
    if (lastedited != "gamma")copy = newref;
    cv::Mat image = copy.clone();

    cv::Mat lookupTable(1, 256, CV_8U);

    for (int i = 0; i < 256; ++i) {
        lookupTable.at<uchar>(0, i) = cv::saturate_cast<uchar>(pow(i / 255.0, *gamma) * 255.0);
    }

    cv::LUT(image, lookupTable, image);
    newref = image;
    texID = get_texturID(image);
}

void Image::BoxBlur(int* blurRadius) {
    cv::Mat image = copy.clone();

    cv::blur(image, image, cv::Size(*blurRadius,*blurRadius));
    
    newref = image;
    texID = get_texturID(image);
}

void Image::GaussBlur(int* blurRadius, float * dx, float *dy) {
    cv::Mat image = copy.clone();

    cv::GaussianBlur(image, image, cv::Size(*blurRadius, *blurRadius),*dx, *dy );

    newref = image;
    texID = get_texturID(image);
}

void Image::MedianBlur(int* blurRadius) {
    cv::Mat image = copy.clone();

    cv::medianBlur(image, image, *blurRadius);

    newref = image;
    texID = get_texturID(image);
}


