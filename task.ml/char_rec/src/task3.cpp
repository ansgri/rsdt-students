#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <stdio.h>

#include "opencv2/opencv.hpp"

#include "fann.h"
#include "floatfann.h"

void readJPGs(std::vector<std::pair<char, cv::Mat> >& vect, int start_num, int end_num, char label, const char *folder) {
    for (int i = start_num; i < end_num; ++i) {
        std::string num = std::to_string(i);
        if (num.length() < 3) {
            for (int j = 0; j <= 3 - num.length(); ++j) {
                num.insert(0, "0");
            }
        }
        std::string filename = "_00" + num + ".jpg";
        filename.insert(0, 1, label);
        filename.insert(0, folder);
        cv::Mat img = cv::imread(filename, CV_LOAD_IMAGE_GRAYSCALE);
        vect.push_back(std::make_pair(label, img));
    }
}

void writeMatToFile(cv::Mat& m, std::ofstream& fout) {
    if (!fout) {
        std::cout << "File Can Not Be Opened" << std::endl;
        return;
    }

    for (int i = 0; i < m.rows; ++i) {
        for (int j = 0; j < m.cols; ++j) {
            fout << (int)m.at<uchar>(i,j) << ' ';
        }
    }
    fout << std::endl;
}

void createData(const char *filename,
                int pixel_number,
                std::pair<int, int> first_num,
                std::pair<int, int> second_num,
                char first_label,
                char second_label,
                const char *folder_name) {
    std::ofstream file(filename, std::ofstream::out);
    std::vector<std::pair<char, cv::Mat> > vect;
    readJPGs(vect, first_num.first, first_num.second, first_label, folder_name);
    readJPGs(vect, first_num.first, first_num.second, second_label, folder_name);
    std::random_shuffle(vect.begin(), vect.end());

    file << vect.size() << " " << pixel_number << " " << 1 << std::endl;
    for (int i = 0; i < vect.size(); ++i) {
        writeMatToFile(vect[i].second, file);
        file << (vect[i].first == first_label ? 0 : 1) << std::endl;
        // 'X' -> 0
        // 'Y' -> 1
    }
    file.close();
}


int main() {
    // creation of train.txt and test.txt file

    int pixel_number = 21 * 28;
    createData("train.txt", pixel_number, std::make_pair(0, 100), std::make_pair(0, 100), 'X', 'Y', "train/");
    createData("test.txt", pixel_number, std::make_pair(100, 188), std::make_pair(100, 199), 'X', 'Y', "test/");




    // training NN
    const unsigned int num_input = pixel_number;
    const unsigned int num_output = 1;
    const unsigned int num_layers = 3;
    const unsigned int num_neurons_hidden = 400;
    const unsigned int num_epochs = 1000;
    
    struct fann *ann = fann_create_standard(num_layers, num_input, num_neurons_hidden, num_output);
    fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC);
    fann_set_activation_function_output(ann, FANN_SIGMOID_SYMMETRIC);

    struct fann_train_data *train_data, *test_data;
    train_data = fann_read_train_from_file("train.txt");
    test_data = fann_read_train_from_file("test.txt");

    std::vector<float> mse_train;
    std::vector<float> mse_test;

    for (int i = 0; i < num_epochs; ++i) {
        std::cout << "training epoch number " << i << std::endl;
        fann_train_epoch(ann, train_data);
        mse_train.push_back(fann_test_data(ann, train_data));
        mse_test.push_back(fann_test_data(ann, test_data));
    }

    fann_destroy(ann);
    std::ofstream output_test("output_test.txt", std::ofstream::out);
    std::ofstream output_train("output_train.txt", std::ofstream::out);

    for (int i = 0; i < mse_train.size(); ++i) {
        // printf("\nTrain error: %f, Test error: %f\n", mse_train[i], mse_test[i]);
        output_test << mse_test[i] << std::endl;
        output_train << mse_train[i] << std::endl;
    }
    return 0;
}





