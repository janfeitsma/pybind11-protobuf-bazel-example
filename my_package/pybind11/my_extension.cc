#include <iostream>
#include <iomanip>
#include <pybind11/pybind11.h>

#include "google/protobuf/message.h"
#include "pybind11_protobuf/native_proto_caster.h"

#include "my_package/proto/example.pb.h"

namespace py = ::pybind11;

// In real use, these 2 functions would probably be defined in a python-agnostic library.
MyMessage ReturnMyMessage() { 
  MyMessage msg;
  msg.set_my_string("Hello World");
  return msg;
 }

void TakeMyMessage(const MyMessage& in) {
    std::cout << "Took Message" << std::endl;
}

void MutateMessage(MyMessage* in_out) {
  in_out->set_my_string("I changed the value");
}


#include <google/protobuf/util/time_util.h>
double GetCurrentTime() {
    auto now = std::chrono::high_resolution_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::microseconds>(now).count() * 1e-6;
}

// Function that modifies the Protobuf object in-place
void f(google::protobuf::Message* in_out)
{
    std::cout << std::fixed << std::setw(17) << std::setprecision(6) << GetCurrentTime() << " c++ f start " << std::endl;
    if (in_out) {

        // Get the descriptors for the protobuf message
        const google::protobuf::Reflection* reflection = in_out->GetReflection();
        const google::protobuf::Descriptor* descriptor = in_out->GetDescriptor();
        const google::protobuf::FieldDescriptor* v_field = descriptor->FindFieldByName("v");
        const google::protobuf::FieldDescriptor* k_field = descriptor->FindFieldByName("k");
        const google::protobuf::FieldDescriptor* n_field = descriptor->FindFieldByName("n");
        const google::protobuf::FieldDescriptor* s_field = descriptor->FindFieldByName("s");

        // Check if the "v" field exists and is a repeated field
        if (v_field && v_field->is_repeated()) {

            // Get the size of the repeated field "v"
            int field_size = reflection->FieldSize(*in_out, v_field);

            // Calculate the sum of the values in the repeated field "v"
            int32_t k = reflection->GetInt32(*in_out, k_field);
            int32_t n = reflection->GetInt32(*in_out, n_field);
            int64_t sum = 0;
            while (k--) { // repeat k times
                for (int i = 0; i < std::min(n, field_size); ++i) {
                    sum += reflection->GetRepeatedInt32(*in_out, v_field, i);
                }
            }
            std::cout << "field_size: " << field_size << std::endl;
            std::cout << "Sum of values in v: " << sum << std::endl;

            in_out->GetReflection()->SetInt64(in_out, s_field, sum);

        } else {
            // Handle the case when the "v" field does not exist or is not a repeated field
            std::cerr << "ERROR: Field 'v' not found or not a repeated field." << std::endl;
        }

    }
    std::cout << std::fixed << std::setw(17) << std::setprecision(6) << GetCurrentTime() << " c++ f end " << std::endl;
}


PYBIND11_MODULE(my_extension, m) {
  pybind11_protobuf::ImportNativeProtoCasters();
  m.def("return_my_message", &ReturnMyMessage);
  m.def("take_my_message", &TakeMyMessage, pybind11::arg("in"));
  m.def("mutate_message", [](MyMessage in) {
    MutateMessage(&in);
    return in;
  },  pybind11::arg("in"));

    // Define the C++ function 'f' using py::arg and py::call_guard
    m.def("f",
        [](MyMessage in_out) {
            f(&in_out);
            return in_out;
        },
        pybind11::arg("in_out"),
        pybind11::call_guard<py::gil_scoped_release>()
    );
}

