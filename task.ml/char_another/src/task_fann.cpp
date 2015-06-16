#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <stdio.h>
#include <dirent.h>

#include "opencv2/opencv.hpp"

#include "fann.h"
#include "floatfann.h"

void read_jpgs(std::vector<std::pair<std::string, cv::Mat> >& vect, std::string label, const char *folder) {
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(folder)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_name[0] != '.') {
                cv::Mat img = cv::imread(std::string(folder) + ent->d_name, CV_LOAD_IMAGE_GRAYSCALE);
                vect.push_back(std::make_pair(label, img));
            }
        }
        closedir (dir);
    } else {
        std::cout << "Can not open directory " << folder << std::endl;
    }
}

void write_mat_to_file(cv::Mat& m, std::ofstream& fout) {
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

void create_data(std::string input_folder,
                 std::string filename,
                 int pixel_number,
                 std::string first_label,
                 std::string second_label
                ) {
    std::ofstream file(filename, std::ofstream::out);
    std::vector<std::pair<std::string, cv::Mat> > vect;
    read_jpgs(vect, first_label, (input_folder + first_label + "/").c_str());
    read_jpgs(vect, second_label, (input_folder + second_label + "/").c_str());
    std::random_shuffle(vect.begin(), vect.end());

    file << vect.size() << " " << pixel_number << " " << 1 << std::endl;
    for (int i = 0; i < vect.size(); ++i) {
        write_mat_to_file(vect[i].second, file);
        file << (vect[i].first == first_label ? 0 : 1) << std::endl;
    }
    file.close();
}

void train_nn(std::string output_folder, int pixel_number, const unsigned int num_epochs) {
    const unsigned int num_input = pixel_number;
    const unsigned int num_output = 1;
    const unsigned int num_layers = 3;
    const unsigned int num_neurons_hidden = 400;
    
    struct fann *ann = fann_create_standard(num_layers, num_input, num_neurons_hidden, num_output);
    fann_set_activation_function_hidden(ann, FANN_SIGMOID);
    fann_set_activation_function_output(ann, FANN_SIGMOID);

    struct fann_train_data *train_data, *test_data;
    train_data = fann_read_train_from_file((output_folder + "train.txt").c_str());
    test_data = fann_read_train_from_file((output_folder + "test.txt").c_str());

    std::vector<float> err_rate_train;
    std::vector<float> err_rate_test;
    fann_type *calc_out;
    int calc_out_int;

    for (int i = 0; i < num_epochs; ++i) {
        std::cout << "training epoch number " << i << std::endl;
        fann_train_epoch(ann, train_data);
        int err_num = 0;
        for (unsigned int j = 0; j < train_data->num_data; ++j) {
            calc_out = fann_run(ann, train_data->input[j]);
            calc_out_int = (int)round(calc_out[0]);
            // std::cout << "Real value = " << train_data->output[j][0] << ", answer = " << calc_out_int << std::endl;
            if (calc_out_int != train_data->output[j][0]) {
                ++err_num;
            }
        }
        err_rate_train.push_back(1.0 * err_num / train_data->num_data);

        err_num = 0;
        for (unsigned int j = 0; j < test_data->num_data; ++j) {
            calc_out = fann_test(ann, test_data->input[j], test_data->output[j]);
            calc_out_int = (int)round(calc_out[0]);
            // std::cout << "Real value = " << test_data->output[j][0] << ", answer = " << calc_out_int << std::endl;
            if (calc_out_int != test_data->output[j][0]) {
                ++err_num;
            }
        }
        err_rate_test.push_back(1.0 * err_num / test_data->num_data);
    }

    fann_destroy(ann);
    std::ofstream output_test(output_folder + "output_test.txt", std::ofstream::out);
    std::ofstream output_train(output_folder + "output_train.txt", std::ofstream::out);

    for (int i = 0; i < num_epochs; ++i) {
        // printf("Train error: %f, Test error: %f\n", err_rate_train[i], err_rate_test[i]);
        output_test << err_rate_train[i] << std::endl;
        output_train << err_rate_test[i] << std::endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 6) {
        std::cout << "Wrong number of arguments. Given " << (argc - 1) << ", expected 5\n";
        std::cout << "Usage:" << std::endl;
        std::cout << "1) path to input folder with test and train folders from archives" << std::endl;
        std::cout << "2) first symbol" << std::endl;
        std::cout << "3) second symbol" << std::endl;
        std::cout << "4) path to output folder" << std::endl;
        std::cout << "5) number of epochs to train" << std::endl;
        return -1;
    }

    std::string input_folder = argv[1];
    std::string char1 = argv[2];
    std::string char2 = argv[3];
    std::string output_folder = argv[4];
    unsigned int num_epochs = atoi(argv[5]);

    if (input_folder[input_folder.length() - 1] != '/') {
        input_folder.append("/");
    }
    if (output_folder[output_folder.length() - 1] != '/') {
        output_folder.append("/");
    }

    int pixel_number = 21 * 28;
    create_data(input_folder + "train/", output_folder + "train.txt", pixel_number, char1, char2);
    create_data(input_folder + "test/", output_folder + "test.txt", pixel_number, char1, char2);

    train_nn(output_folder, pixel_number, num_epochs);
    return 0;
}