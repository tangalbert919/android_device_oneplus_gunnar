/*
 * Copyright (C) 2022 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include <android-base/logging.h>
#include <android-base/properties.h>

#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>

using android::base::GetProperty;

constexpr const char* BUILD_DESCRIPTION = "GN2200-user 12 RKQ1.211119.001 eeacd3-1dc8a-89f release-keys";
constexpr const char* BUILD_FINGERPRINT = "OnePlus/GN2200/OP515AL1:12/RKQ1.211119.001/eeacd3-1dc8a-89f:user/release-keys";

constexpr const char* RO_PROP_SOURCES[] = {
    nullptr,
    "bootimage.",
    "odm.",
    "odm_dlkm.",
    "product.",
    "system.",
    "system_dlkm.",
    "system_ext.",
    "vendor.",
    "vendor_dlkm.",
};

/*
 * SetProperty does not allow updating read only properties and as a result
 * does not work for our use case. Write "OverrideProperty" to do practically
 * the same thing as "SetProperty" without this restriction.
 */
void OverrideProperty(const char* name, const char* value) {
    size_t valuelen = strlen(value);

    prop_info* pi = (prop_info*)__system_property_find(name);
    if (pi != nullptr) {
        __system_property_update(pi, value, valuelen);
    } else {
        __system_property_add(name, strlen(name), value, valuelen);
    }
}

void OverrideCarrierProperties() {
    const auto ro_prop_override = [](const char* source, const char* prop, const char* value,
                                     bool product) {
        std::string prop_name = "ro.";

        if (product) prop_name += "product.";
        if (source != nullptr) prop_name += source;
        if (!product) prop_name += "build.";
        prop_name += prop;

        OverrideProperty(prop_name.c_str(), value);
    };

    for (const auto& source : RO_PROP_SOURCES) {
        ro_prop_override(source, "model", "GN2200", true);
        ro_prop_override(source, "device", "OP515AL1", true);
        ro_prop_override(source, "fingerprint", BUILD_FINGERPRINT, false);
    }
    ro_prop_override(nullptr, "product", "GN2200", false);
    ro_prop_override(nullptr, "description", BUILD_DESCRIPTION, false);
}

/*
 * Only for read-only properties. Properties that can be wrote to more
 * than once should be set in a typical init script (e.g. init.oplus.hw.rc)
 * after the original property has been set.
 */
void vendor_load_properties() {
    //auto device = GetProperty("ro.product.product.device", "");
    auto prjname = std::stoi(GetProperty("ro.boot.prjname", "0"));

    // T-Mobile (20826)
    if (prjname == 20826)
        OverrideCarrierProperties();
}
