// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_DIAGNOSTIC_DATA_RESULT_H_
#define FRONTEND_DIAGNOSTIC_DATA_RESULT_H_

#include <cstdint>
#include <new>
#include <string>
#include <type_traits>
#include <utility>

#include "core/check.h"
#include "frontend/diagnostic/base/diagnostic_export.h"

namespace diagnostic {

template <typename T, typename E>
class Result;

template <typename T>
class DIAGNOSTIC_EXPORT Ok {
 public:
  explicit Ok(T&& value) : value_(std::move(value)) {}
  explicit Ok(const T& value) : value_(value) {}

  T into_ok() && { return std::move(value_); }
  const T& value() const& { return value_; }

 private:
  T value_;
};

// no payload to return; kept for api symmetry
template <>
class DIAGNOSTIC_EXPORT Ok<void> {
 public:
  Ok() = default;
  inline void into_ok() && noexcept {}
};

template <typename E>
class DIAGNOSTIC_EXPORT Err {
 public:
  explicit Err(E&& error) : error_(std::move(error)) {}
  explicit Err(const E& error) : error_(error) {}

  E into_err() && { return std::move(error_); }
  const E& error() const& { return error_; }

 private:
  E error_;
};

template <typename T>
Ok<std::decay_t<T>> create_ok(T&& value) {
  return Ok<std::decay_t<T>>(std::forward<T>(value));
}

inline Ok<void> create_ok() {
  return Ok<void>();
}

template <typename E>
Err<std::decay_t<E>> create_err(E&& error) {
  return Err<std::decay_t<E>>(std::forward<E>(error));
}

template <typename T, typename E>
class DIAGNOSTIC_EXPORT Result {
 public:
  explicit Result(Ok<T>&& ok_value) : is_ok_(true) {
    new (&storage_.ok_value_) T(std::move(ok_value).into_ok());
  }

  explicit Result(Err<E>&& err_value) : is_ok_(false) {
    new (&storage_.err_value_) E(std::move(err_value).into_err());
  }

  ~Result() {
    if (is_ok_) {
      storage_.ok_value_.~T();
    } else {
      storage_.err_value_.~E();
    }
  }

  Result(const Result& other) : is_ok_(other.is_ok_) {
    if (is_ok_) {
      new (&storage_.ok_value_) T(other.storage_.ok_value_);
    } else {
      new (&storage_.err_value_) E(other.storage_.err_value_);
    }
  }

  Result(Result&& other) noexcept : is_ok_(other.is_ok_) {
    if (is_ok_) {
      new (&storage_.ok_value_) T(std::move(other.storage_.ok_value_));
    } else {
      new (&storage_.err_value_) E(std::move(other.storage_.err_value_));
    }
  }

  Result& operator=(const Result& other) {
    if (this != &other) {
      this->~Result();
      new (this) Result(other);
    }
    return *this;
  }

  Result& operator=(Result&& other) noexcept {
    if (this != &other) {
      this->~Result();
      new (this) Result(std::move(other));
    }
    return *this;
  }

  inline bool is_ok() const { return is_ok_; }

  inline bool is_err() const { return !is_ok_; }

  const T& unwrap() const& {
    DCHECK(is_ok_);
    return storage_.ok_value_;
  }

  T unwrap() && {
    DCHECK(is_ok_);
    return std::move(storage_.ok_value_);
  }

  const E& unwrap_err() const& {
    DCHECK(!is_ok_);
    return storage_.err_value_;
  }

  E unwrap_err() && {
    DCHECK(!is_ok_);
    return std::move(storage_.err_value_);
  }

  T unwrap_or(T&& default_value) && {
    if (is_ok_) {
      return std::move(storage_.ok_value_);
    } else {
      return std::move(default_value);
    }
  }

  const T& unwrap_or(const T& default_value) const& {
    if (is_ok_) {
      return storage_.ok_value_;
    } else {
      return default_value;
    }
  }

  const T& expect(const std::string& msg) const& {
    DCHECK(is_ok_) << msg << ": " << storage_.err_value_;
    return storage_.ok_value_;
  }

  T expect(const std::string& msg) && {
    DCHECK(is_ok_) << msg << ": " << std::move(storage_.err_value_);
    return std::move(storage_.ok_value_);
  }

  template <typename F>
  auto map(F&& f) && {
    using U = std::invoke_result_t<F, T>;
    if (is_ok_) {
      return create_ok(std::forward<F>(f)(std::move(storage_.ok_value_)));
    } else {
      return Result<U, E>(std::move(*this).unwrap_err());
    }
  }

  template <typename F>
  auto map(F&& f) const& {
    using U = std::invoke_result_t<F, const T&>;
    if (is_ok_) {
      return create_ok(std::forward<F>(f)(storage_.ok_value_));
    } else {
      return Result<U, E>(storage_.err_value_);
    }
  }

  template <typename F>
  auto map_err(F&& f) && {
    using F_ret_type = std::invoke_result_t<F, E>;
    if (!is_ok_) {
      return create_err(std::forward<F>(f)(std::move(storage_.err_value_)));
    } else {
      return Result<T, F_ret_type>(std::move(*this).unwrap());
    }
  }

  template <typename F>
  auto map_err(F&& f) const& {
    using F_ret_type = std::invoke_result_t<F, const E&>;
    if (!is_ok_) {
      return create_err(std::forward<F>(f)(storage_.err_value_));
    } else {
      return Result<T, F_ret_type>(storage_.ok_value_);
    }
  }

  template <typename F>
  auto and_then(F&& f) && {
    using NextResult = std::invoke_result_t<F, T>;
    static_assert(std::is_same_v<typename NextResult::ErrorType, E>,
                  "error types must match for and_then");
    if (is_ok_) {
      return std::forward<F>(f)(std::move(storage_.ok_value_));
    } else {
      return NextResult(std::move(*this).unwrap_err());
    }
  }

  template <typename F>
  auto and_then(F&& f) const& {
    using NextResult = std::invoke_result_t<F, const T&>;
    static_assert(std::is_same_v<typename NextResult::ErrorType, E>,
                  "error types must match for and_then");
    if (is_ok_) {
      return std::forward<F>(f)(storage_.ok_value_);
    } else {
      return NextResult(storage_.err_value_);
    }
  }

  using ValueType = T;
  using ErrorType = E;

 private:
  union Storage {
    T ok_value_;
    E err_value_;

    Storage() {}
    ~Storage() {}
  };

  Storage storage_;
  bool is_ok_ : 1;
};

template <typename E>
class DIAGNOSTIC_EXPORT Result<void, E> {
 public:
  // construct ok (no payload)
  explicit Result(Ok<void>&&) : is_ok_(true) {}

  // construct err
  explicit Result(Err<E>&& err_value) : is_ok_(false) {
    new (&storage_.err_value_) E(std::move(err_value).into_err());
  }

  ~Result() {
    if (!is_ok_) {
      storage_.err_value_.~E();
    }
  }

  Result(const Result& other) : is_ok_(other.is_ok_) {
    if (!is_ok_) {
      new (&storage_.err_value_) E(other.storage_.err_value_);
    }
  }

  Result(Result&& other) noexcept : is_ok_(other.is_ok_) {
    if (!is_ok_) {
      new (&storage_.err_value_) E(std::move(other.storage_.err_value_));
    }
    // keep other's active member unchanged; both destructors will handle their
    // own states
  }

  Result& operator=(const Result& other) {
    if (this != &other) {
      this->~Result();
      new (this) Result(other);
    }
    return *this;
  }

  Result& operator=(Result&& other) noexcept {
    if (this != &other) {
      this->~Result();
      new (this) Result(std::move(other));
    }
    return *this;
  }

  // state queries
  inline bool is_ok() const { return is_ok_; }
  inline bool is_err() const { return !is_ok_; }

  // unwrap ok (no payload) - no-op with check
  void unwrap() && { DCHECK(is_ok_); }
  void expect(const std::string& msg) const& {
    DCHECK(is_ok_) << msg << ": " << storage_.err_value_;
  }

  // unwrap error
  const E& unwrap_err() const& {
    DCHECK(!is_ok_);
    return storage_.err_value_;
  }
  E unwrap_err() && {
    DCHECK(!is_ok_);
    return std::move(storage_.err_value_);
  }

  // map: void -> U by invoking f() only on ok
  template <typename F>
  auto map(F&& f) && {
    using U = std::invoke_result_t<F>;
    if (is_ok_) {
      return create_ok(std::forward<F>(f)());
    } else {
      return Result<U, E>(Err<E>(std::move(storage_.err_value_)));
    }
  }
  template <typename F>
  auto map(F&& f) const& {
    using U = std::invoke_result_t<F>;
    if (is_ok_) {
      return create_ok(std::forward<F>(f)());
    } else {
      return Result<U, E>(Err<E>(storage_.err_value_));
    }
  }

  // map_err: transform error only
  template <typename F>
  auto map_err(F&& f) && {
    using E2 = std::invoke_result_t<F, E>;
    if (!is_ok_) {
      return create_err(std::forward<F>(f)(std::move(storage_.err_value_)));
    } else {
      return Result<void, E2>(create_ok());
    }
  }
  template <typename F>
  auto map_err(F&& f) const& {
    using E2 = std::invoke_result_t<F, const E&>;
    if (!is_ok_) {
      return create_err(std::forward<F>(f)(storage_.err_value_));
    } else {
      return Result<void, E2>(create_ok());
    }
  }

  // and_then: chain to next result when ok; f: () -> Result<*, E>
  template <typename F>
  auto and_then(F&& f) && {
    using NextResult = std::invoke_result_t<F>;
    static_assert(std::is_same_v<typename NextResult::ErrorType, E>,
                  "error types must match for and_then");
    if (is_ok_) {
      return std::forward<F>(f)();
    } else {
      return NextResult(Err<E>(std::move(storage_.err_value_)));
    }
  }
  template <typename F>
  auto and_then(F&& f) const& {
    using NextResult = std::invoke_result_t<F>;
    static_assert(std::is_same_v<typename NextResult::ErrorType, E>,
                  "error types must match for and_then");
    if (is_ok_) {
      return std::forward<F>(f)();
    } else {
      return NextResult(Err<E>(storage_.err_value_));
    }
  }

  using ValueType = void;
  using ErrorType = E;

 private:
  union Storage {
    char dummy_;
    E err_value_;
    Storage() {}
    ~Storage() {}
  } storage_;
  bool is_ok_ : 1;
};

}  // namespace diagnostic

#endif  // FRONTEND_DIAGNOSTIC_DATA_RESULT_H_
