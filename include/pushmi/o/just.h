/*
 * Copyright 2018-present Facebook, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once

#include <pushmi/o/extension_operators.h>
#include <pushmi/o/submit.h>
#include <pushmi/single_sender.h>

namespace pushmi {

namespace operators {

PUSHMI_INLINE_VAR constexpr struct just_fn {
 private:
  struct sender_base : single_sender<> {
    using properties = property_set<
        is_sender<>,
        is_single<>,
        is_always_blocking<>>;
  };
  template <class... VN>
  struct impl {
    std::tuple<VN...> vn_;
    PUSHMI_TEMPLATE(class In, class Out)
    (requires ReceiveValue<Out, VN...>) //
        void
        operator()(In&&, Out&& out) {
      ::pushmi::apply(
          ::pushmi::set_value,
          std::tuple_cat(std::tuple<Out&>{out}, std::move(vn_)));
      set_done(out);
    }
  };

 public:
  PUSHMI_TEMPLATE(class... VN)
  (requires And<SemiMovable<VN>...>) //
      auto
      operator()(VN... vn) const {
    return make_single_sender(
        sender_base{}, impl<VN...>{std::tuple<VN...>{std::move(vn)...}});
  }
} just{};
} // namespace operators

} // namespace pushmi
