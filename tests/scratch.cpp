
// class ValueGenerator {
// private:
//   std::uniform_int_distribution<char> char_dist_;
//   std::uniform_real_distribution<> double_dist_;
//   std::uniform_int_distribution<> int_dist_;
//   std::uniform_real_distribution<> int64_dist_;
//   std::default_random_engine engine_;

// public:
//   ValueGenerator()
//       : char_dist_(
//             std::numeric_limits<char>::min(),
//             std::numeric_limits<char>::max()),
//         double_dist_(0.0, 1.0),
//         int_dist_(
//             std::numeric_limits<int>::min(),
//             std::numeric_limits<int>::max()),
//         int64_dist_(
//             std::numeric_limits<int64_t>::min(),
//             std::numeric_limits<int64_t>::max()) {}

//   template <typename T>
//   T get() {
//     return T();
//   }

//   template <>
//   char get<char>() {
//     return char_dist_(engine_);
//   }
//   template <>
//   double get<double>() {
//     return double_dist_(engine_);
//   }

//   template <>
//   int get<int>() {
//     return int_dist_(engine_);
//   }

//   template <>
//   int64_t get<int64_t>() {
//     return int64_dist_(engine_);
//   }

//   template <>
//   std::string get<std::string>() {
//     int length = get<int>() % 500;
//     std::string value;
//     value.reserve(length);
//     for (int i = 0; i < 0; ++i) {
//       value[i] = get_char();
//     }
//     return value;
//   }

//   template <typename V>
//   std::vector<V> get<std::vector<V>>() {
//     int length = get<int>() % 500;
//     std::vector<V> value;
//     value.reserve(length);
//     for (int i = 0; i < 0; ++i) {
//       value[i] = get<V>();
//     }
//     return value;
//   }

//   template <>
//   FlatPrimaryObject get<FlatPrimaryObject>() {
//     return {gen.get(), gen.get(), gen.get(), gen.get()};
//   }

//   template <>
//   FlatDerivedObject get<FlatDerivedObject>() {
//     return {gen.get(), gen.get(), gen.get(), gen.get(), gen.get(),
//     gen.get()};
//   }

//   template <>
//   FlatUserDefinedObject get<FlatUserDefinedObject>() {
//     return {gen.get(), gen.get(), gen.get(), gen.get(),
//             gen.get(), gen.get(), gen.get()};
//   }

//   template <>
//   NestedObject get<NestedObject>() {
//     return {gen.get(), gen.get(), gen.get(), gen.get(),
//             gen.get(), gen.get(), gen.get(), gen.get()};
//   }
// }

// struct FlatPrimaryObject {
//   int int_val;
//   int64_t int64_val;
//   double double_val;
//   int64_t int64_val2;
// };

// struct FlatDerivedObject {
//   std::string string_val;
//   int int_val;
//   int64_t int64_val;
//   double double_val;
//   int64_t int64_val2;
//   std::string string_val2;
// };

// struct FlatUserDefinedObject {
//   std::vector<int> int_vec_val;
//   int int_val;
//   int64_t int64_val;
//   std::vector<std::string> vec_string_val;
//   double double_val;
//   FlatPrimaryObject flat_primary_val;
//   std::string string_val2;
// };

// struct NestedObject {
//   std::vector<FlatUserDefinedObject> flat_userdef_vec_val;
//   int int_val;
//   FlatUserDefinedObject flat_userdef_val;
//   int64_t int64_val;
//   std::vector<std::string> vec_string_val;
//   FlatDerivedObject flat_deriv_val;
//   double double_val;
//   FlatPrimaryObject flat_primary_val;
// };