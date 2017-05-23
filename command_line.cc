#include <iostream>
#include <string>

#include <boost/any.hpp>
#include <boost/program_options.hpp>

namespace {
const size_t ERROR_IN_COMMAND_LINE = 1;
const size_t SUCCESS = 0;
const size_t ERROR_UNHANDLED_EXCEPTION = 2;
}

template <typename T, typename Parameter> class NamedType {
  public:
    explicit NamedType(T const &value) : value_(value) {}
    T &get() { return value_; }
    T const &get() const { return value_; }

  private:
    T value_;
};

namespace parameters {
using SampleRate = NamedType<int, struct SampleRateTag>;

void validate(boost::any &v, std::vector<std::string> const &values,
              SampleRate *, int) {
    using namespace boost::program_options;
    // Make sure no previous assignment to 'v' was made.
    validators::check_first_occurrence(v);

    // Extract the first string from 'values'. If there is more than
    // one string, it's an error, and exception will be thrown.
    std::string const &s = validators::get_single_string(values);

    const std::vector<std::string> validSampleRates = {
        "96000", "48000", "44100", "32000", "24000", "16000", "8000"};
    if (std::find(std::begin(validSampleRates), std::end(validSampleRates),
                  s) != std::end(validSampleRates)) {
        int sampleRate = boost::lexical_cast<int>(s);
        v = boost::any(SampleRate(sampleRate));
    } else {
        throw validation_error(validation_error::invalid_option_value);
    }
}
}

int main(int argc, char **argv) {
    try {
        namespace po = boost::program_options;
        po::options_description desc("Parameters");
        desc.add_options()("help,h", "Display this help")(
            "sr", po::value<parameters::SampleRate>(), "set the sample rate");

        po::variables_map vm;
        try {
            po::store(po::parse_command_line(argc, argv, desc), vm);

            if (vm.count("help")) {
                std::cout << "Basic Command Line Parameter App" << std::endl
                          << desc << std::endl;
                return SUCCESS;
            }

            po::notify(vm);

            if (vm.count("sr")) {
                std::cout << "sample rate is "
                          << vm["sr"].as<parameters::SampleRate>().get()
                          << std::endl;
            }

        } catch (po::error &e) {
            std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
            std::cerr << desc << std::endl;
            return ERROR_IN_COMMAND_LINE;
        }

        // application code here //

    } catch (std::exception &e) {
        std::cerr << "Unhandled Exception reached the top of main: " << e.what()
                  << ", application will now exit" << std::endl;
        return ERROR_UNHANDLED_EXCEPTION;
    }

    return SUCCESS;
}
