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
void f(google::protobuf::Message* in_out, const google::protobuf::Message& in) {
    std::cout << std::fixed << std::setw(17) << std::setprecision(6) << GetCurrentTime() << " c++ f start " << std::endl;
    if (in_out) {

        // some deprecated warnings ... meh.

        const google::protobuf::Reflection* in_reflection = in.GetReflection();
        const google::protobuf::FieldDescriptor* v_field = in.GetDescriptor()->FindFieldByName("v");
        const google::protobuf::FieldDescriptor* k_field = in.GetDescriptor()->FindFieldByName("k");
        const google::protobuf::FieldDescriptor* s_field = in.GetDescriptor()->FindFieldByName("s");

        const auto& in_v = in_reflection->GetRepeatedField<int32_t>(in, v_field);
        auto* in_out_v = in_out->GetReflection()->MutableRepeatedField<int32_t>(in_out, v_field);

        while (in_out_v->size() < in_v.size()) { // TODO optimize
            in_out_v->Add(0);
        }
        std::cout << std::fixed << std::setw(17) << std::setprecision(6) << GetCurrentTime() << " c++ f allocated " << std::endl;

        for (int i = 0; i < in_v.size(); ++i) {
            (*in_out_v)[i] += in_v.Get(i);
        }

        const int32_t in_k = in_reflection->GetInt32(in, k_field);
        for (int k = 1; k < in_k; ++k) {
            for (int i = 0; i < in_v.size(); ++i) {
                (*in_out_v)[i] += in_v.Get(i);
            }
        }

        int64_t total = 0;
        for (int i = 0; i < in_out_v->size(); ++i) {
            total += (*in_out_v)[i];
        }

        in_out->GetReflection()->SetInt64(in_out, s_field, total);
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
        [](MyMessage in_out, MyMessage in) {
            f(&in_out, in);
            return in_out;
        },
        pybind11::arg("in_out"),
        pybind11::arg("in"),
        pybind11::call_guard<py::gil_scoped_release>()
    );
}

