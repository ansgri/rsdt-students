#include <floatfann.h>
#include <fann_cpp.h>
#include <cstdio>
#include <stdexcept>
#include <string>
#include <boost/lexical_cast.hpp>


static int print_callback(FANN::neural_net &net, FANN::training_data &train,
                          unsigned max_epochs, unsigned epochs_between_reports,
                          float desired_error, unsigned epochs, void *user_data)
{
  printf("N: %d  epochs: %6d  err: %1.5f\n",
         net.get_total_neurons(),
         epochs,
         net.get_MSE());
  net.save("_digits_float." + boost::lexical_cast<std::string>(getpid()) + ".fann");
  return 0;
}

static void init_nn(FANN::neural_net & net)
{
  // net.create_standard(3,  // num_layers
  //                     64,  // num_input
  //                     30,  // num_hidden
  //                     10); // num_output

  net.create_shortcut(2,  // num_layers
                      64,  // num_input
                      10); // num_output

  net.set_activation_function_hidden(FANN::SIGMOID_STEPWISE);
  net.set_activation_steepness_hidden(1.0);

  net.set_activation_function_output(FANN::SIGMOID_STEPWISE);
  net.set_activation_steepness_output(1.0);

  net.set_training_algorithm(FANN::TRAIN_RPROP);
  net.set_learning_rate(0.3f);
  net.set_cascade_output_change_fraction(0.001);
}

static void test_nn(FANN::neural_net & net,
                    FANN::training_data & data)
{
  for (unsigned i = 0; i < data.length_train_data(); ++i)
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
}

static void run_nn()
{
  FANN::neural_net net;
  init_nn(net);
  // net.print_parameters();


  FANN::training_data data;
  std::string dataset_filename = "../testdata/digits.data";
  if (!data.read_train_from_file(dataset_filename))
    throw std::runtime_error("Cannot read data from " + dataset_filename);

  // Training
  net.init_weights(data);
  // net.randomize_weights(-1, 1);

  net.set_callback(print_callback, NULL);

  float const desired_error = 0.0001f;
  unsigned const max_iterations = 300000;
  unsigned const iterations_between_reports = 10;
  net.cascadetrain_on_data(data,
                    500,
                    // max_iterations,
                    1,
                    // iterations_between_reports,
                    desired_error);

  // test_nn(net, data);
}

int main(int argc, char **argv)
{
  try
  {
    run_nn
  ();
    return 0;
  }
  catch (std::exception const& e)
  {
    fprintf(stderr, "Exception: %s\n", e.what());
    return 1;
  }
}
