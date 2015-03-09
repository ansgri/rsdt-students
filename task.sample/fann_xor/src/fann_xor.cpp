#include <floatfann.h>
#include <fann_cpp.h>
#include <cstdio>

int print_callback(FANN::neural_net &net, FANN::training_data &train,
    unsigned int max_epochs, unsigned int epochs_between_reports,
    float desired_error, unsigned int epochs, void *user_data)
{
    printf("N: %d  epochs: %6d  err: %1.5f\n",
           net.get_total_neurons(),
           epochs,
           net.get_MSE());
    return 0;
}

// Test function that demonstrates usage of the fann C++ wrapper
void xor_test()
{
    FANN::neural_net net;
    net.create_standard(3,  // num_layers
                        2,  // num_input
                        3,  // num_hidden
                        1); // num_output

    net.set_learning_rate(0.7f);

    net.set_activation_function_hidden(FANN::SIGMOID_SYMMETRIC_STEPWISE);
    net.set_activation_steepness_hidden(1.0);

    net.set_activation_function_output(FANN::SIGMOID_SYMMETRIC_STEPWISE);
    net.set_activation_steepness_output(1.0);

    net.set_training_algorithm(FANN::TRAIN_QUICKPROP);

    // net.print_parameters();

    FANN::training_data data;
    if (data.read_train_from_file("../testdata/fann_xor.data"))
    {
        // Training
        net.init_weights(data);
        // net.randomize_weights(-1, 1);

        net.set_callback(print_callback, NULL);

        float const desired_error = 0.001f;
        unsigned const max_iterations = 300000;
        unsigned const iterations_between_reports = 10000;
        net.train_on_data(data, 
                          max_iterations,
                          iterations_between_reports,
                          desired_error);

        for (unsigned int i = 0; i < data.length_train_data(); ++i)
        {
            // Run the network on the test data
            fann_type *calc_out = net.run(data.get_input()[i]);

            printf("(%.1f, %.1f) -> %.1f  =  %.4f  ~  %.4f\n",
                   data.get_input()[i][0], 
                   data.get_input()[i][1], 
                   data.get_output()[i][0], 
                   *calc_out,
                   fann_abs(*calc_out - data.get_output()[i][0]));
        }

        net.save("xor_float.net");
    }
}

int main(int argc, char **argv)
{
    try
    {
        xor_test();
        return 0;
    }
    catch (std::exception const& e)
    {
        fprintf(stderr, "Exception: %s\n", e.what());
        return 1;
    }
}
