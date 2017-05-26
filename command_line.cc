#include <iostream>
#include <sstream>
#include <string>
#include <cmath>

#include <boost/any.hpp>
#include <boost/program_options.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/filesystem.hpp>

#include <sndfile.hh>

#include "NamedType.hpp"

namespace {
const size_t ERROR_IN_COMMAND_LINE = 1;
const size_t SUCCESS = 0;
const size_t ERROR_UNHANDLED_EXCEPTION = 2;
}

namespace parameters {
    class OptionName {
        public:
            explicit OptionName(std::string const& long_name,
                                std::string const& short_name) : 
                    long_name_(long_name),
                    short_name_(short_name) { complete_name_ = long_name_ + "," + short_name; }
            std::string const& get_long()     const { return long_name_;  }
            std::string const& get_short()    const { return short_name_; }
            std::string const& get_complete() const { return complete_name_; }
        private:
            std::string long_name_;
            std::string short_name_;
            std::string complete_name_;
    };

    const OptionName helpOptionName("help", "h");

    const OptionName frequencyOptionName("frequency", "f");
    using HumanEarableFrequency = NamedType<int, struct HumanEarableFrequencyTag>;
    const int minHumanEarableFrequency = 12;
    const int maxHumanEarableFrequency = 28000;

    void validate(boost::any &v, std::vector<std::string> const &values,
                  HumanEarableFrequency *, int) {
        using namespace boost::program_options;
        validators::check_first_occurrence(v);
    
        std::string const &s = validators::get_single_string(values);

        double humanEarableFrequency = boost::lexical_cast<int>(s);    
    
        if ( (minHumanEarableFrequency <= humanEarableFrequency) && 
             (humanEarableFrequency    <= maxHumanEarableFrequency) ) {
            v = boost::any(HumanEarableFrequency(humanEarableFrequency));
        } else {
            throw validation_error(validation_error::invalid_option_value);
        }
    }

    const OptionName sampleRateOptionName("sample-rate", "s");
    using SampleRate = NamedType<int, struct SampleRateTag>;
    const std::vector<std::string> validSampleRates = { 
            "96000",
            "48000",
            "44100",
            "32000",
            "24000",
            "16000",
    };
    
    void validate(boost::any &v, std::vector<std::string> const &values,
                  SampleRate *, int) {
        using namespace boost::program_options;
        validators::check_first_occurrence(v);
    
        std::string const &s = validators::get_single_string(values);
    
        if (std::find(std::begin(validSampleRates), std::end(validSampleRates), s) != std::end(validSampleRates)) {
            int sampleRate = boost::lexical_cast<int>(s);
            v = boost::any(SampleRate(sampleRate));
        } else {
            throw validation_error(validation_error::invalid_option_value);
        }
    }

    const OptionName audioFormatOptionName("audio-format", "a");
    using AudioFormat = NamedType<std::string, struct AudioFormatTag>;
    const std::vector<std::string> validAudioFormats = {
            "pcm32",
            "pcm24",
            "pcm16",
            "float",
            "double"
    };

    void validate(boost::any &v, std::vector<std::string> const &values,
                  AudioFormat *, int) {
        using namespace boost::program_options;
        validators::check_first_occurrence(v);
    
        std::string const &s = validators::get_single_string(values);
    
        if (std::find(std::begin(validAudioFormats), std::end(validAudioFormats), s) != std::end(validAudioFormats)) {
            v = boost::any(AudioFormat(s));
        } else {
            throw validation_error(validation_error::invalid_option_value);
        }
    }

    const OptionName prefixNameOptionName("prefix-name", "n");
    using PrefixName = NamedType<std::string, struct PrefixNameTag>;

    void validate(boost::any &v, std::vector<std::string> const &values,
                  PrefixName *, int) {
        using namespace boost::program_options;
        validators::check_first_occurrence(v);
    
        std::string const &s = validators::get_single_string(values);
    
        if (boost::filesystem::portable_file_name(s)) {
            v = boost::any(PrefixName(s));
        } else {
            throw validation_error(validation_error::invalid_option_value);
        }
    }
}

int audio_format_to_sf_format(const std::string& audio_format)
{
    if (audio_format == "pcm32")  return SF_FORMAT_PCM_32;
    if (audio_format == "pcm24")  return SF_FORMAT_PCM_24;
    if (audio_format == "pcm16")  return SF_FORMAT_PCM_16;
    if (audio_format == "float")  return SF_FORMAT_FLOAT;
    if (audio_format == "double") return SF_FORMAT_DOUBLE;

    return -1;
}

int main(int argc, char **argv) {
    try {
        namespace po = boost::program_options;
        po::options_description desc("\nCreate a sine wave with these parameters");

        const std::string validSampleRates  = boost::algorithm::join(parameters::validSampleRates,  "|");
        const std::string sampleRatesOptions = validSampleRates + " Hz";
        const std::string validAudioFormats = boost::algorithm::join(parameters::validAudioFormats, "|");

        desc.add_options()
                (parameters::frequencyOptionName.get_complete().c_str(),   po::value<parameters::HumanEarableFrequency>()->value_name(" ")->required(), "Hz")
                (parameters::sampleRateOptionName.get_complete().c_str(),  po::value<parameters::SampleRate>()->value_name(" "),                        sampleRatesOptions.c_str())
                (parameters::audioFormatOptionName.get_complete().c_str(), po::value<parameters::AudioFormat>()->value_name(" "),                       validAudioFormats.c_str())
                (parameters::prefixNameOptionName.get_complete().c_str(),  po::value<parameters::PrefixName>()->value_name(" "),                        "<prefix_name>_<sample-rate>_<audio_format>.wav")
                (parameters::helpOptionName.get_complete().c_str(), "");

        po::variables_map vm;

        // Default values
        double frequency = 440.0;
        //double duration = 10*1.0/frequency;
        double duration = 1;
        int sample_rate = 48000;
        std::string audio_format = "pcm24";
        std::string prefix = "sine";
        try {
            po::store(po::parse_command_line(argc, argv, desc), vm);

            if (vm.count(parameters::helpOptionName.get_long())) {
                std::cout << desc << std::endl;
                return SUCCESS;
            }

            po::notify(vm);

            if (vm.count(parameters::frequencyOptionName.get_long())) {
                frequency = vm[parameters::frequencyOptionName.get_long()].as<parameters::HumanEarableFrequency>().get();
            }

            if (vm.count(parameters::sampleRateOptionName.get_long())) {
                sample_rate = vm[parameters::sampleRateOptionName.get_long()].as<parameters::SampleRate>().get();
            }

            if (vm.count(parameters::audioFormatOptionName.get_long())) {
                audio_format = vm[parameters::audioFormatOptionName.get_long()].as<parameters::AudioFormat>().get();
            }

            if (vm.count(parameters::prefixNameOptionName.get_long())) {
                prefix = vm[parameters::prefixNameOptionName.get_long()].as<parameters::PrefixName>().get();
            }

        } catch (po::error &e) {
            std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
            std::cerr << desc << std::endl;
            return ERROR_IN_COMMAND_LINE;
        }
        double sample_duration = 1.0/sample_rate;
        int sample_count = sample_rate * duration;

        std::stringstream ss;
        ss << prefix;
        ss << "_";
        ss << sample_rate;
        ss << "_";
        ss << audio_format;
        ss << ".wav";
        std::string output_filename = ss.str();

        printf("frequency=%f\n", frequency);
        printf("duration=%f\n", duration);
        printf("sample_rate=%d\n", sample_rate);
        printf("sample_count=%d\n", sample_count);
        printf("sample_duration=%f\n", sample_duration);
        printf("audio_format=%s\n", audio_format.c_str());
        printf("output_filename=%s\n", output_filename.c_str());

        SndfileHandle sndfilehandle(output_filename, SFM_WRITE, (SF_FORMAT_WAV | audio_format_to_sf_format(audio_format)), 1, sample_rate);
        std::vector<double> buffer(sample_count);

        for (int k = 0; k < sample_count; k++) {
            const double pi = 3.14159265358979323846264338;
            buffer[k] = sin(frequency * 2 * k * pi / sample_rate);
            printf("%d: %f: %f\n", k, sample_duration*k, buffer[k]);
        }

        sndfilehandle.write(buffer.data(), sample_count);
    } catch (std::exception &e) {
        std::cerr << "Unhandled Exception reached the top of main: " << e.what()
                  << ", application will now exit" << std::endl;
        return ERROR_UNHANDLED_EXCEPTION;
    }

    return SUCCESS;
}
