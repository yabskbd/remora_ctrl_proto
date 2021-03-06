/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2018-2019 Erik Moqvist
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * This file was generated by cantools version 36.2.0 Thu Aug  5 21:11:38 2021.
 */

#include <string.h>

#include "rmu_ctrl_canbus_data.h"

static inline uint8_t pack_left_shift_u8(
    uint8_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint8_t)((uint8_t)(value << shift) & mask);
}

static inline uint8_t pack_left_shift_u16(
    uint16_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint8_t)((uint8_t)(value << shift) & mask);
}

static inline uint8_t pack_left_shift_u32(
    uint32_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint8_t)((uint8_t)(value << shift) & mask);
}

static inline uint8_t pack_right_shift_u16(
    uint16_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint8_t)((uint8_t)(value >> shift) & mask);
}

static inline uint8_t pack_right_shift_u32(
    uint32_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint8_t)((uint8_t)(value >> shift) & mask);
}

static inline uint16_t unpack_left_shift_u16(
    uint8_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint16_t)((uint16_t)(value & mask) << shift);
}

static inline uint32_t unpack_left_shift_u32(
    uint8_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint32_t)((uint32_t)(value & mask) << shift);
}

static inline uint8_t unpack_right_shift_u8(
    uint8_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint8_t)((uint8_t)(value & mask) >> shift);
}

static inline uint16_t unpack_right_shift_u16(
    uint8_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint16_t)((uint16_t)(value & mask) >> shift);
}

static inline uint32_t unpack_right_shift_u32(
    uint8_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint32_t)((uint32_t)(value & mask) >> shift);
}

int rmu_ctrl_canbus_data_dpfc1_pack(
    uint8_t *dst_p,
    const struct rmu_ctrl_canbus_data_dpfc1_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[1] |= pack_left_shift_u8(src_p->dsl_prtclt_fltr_pssv_rgnrtion_status, 0u, 0x03u);
    dst_p[1] |= pack_left_shift_u8(src_p->dsl_prtclt_fltr_actv_rgnrtion_status, 2u, 0x0cu);

    return (8);
}

int rmu_ctrl_canbus_data_dpfc1_unpack(
    struct rmu_ctrl_canbus_data_dpfc1_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->dsl_prtclt_fltr_pssv_rgnrtion_status = unpack_right_shift_u8(src_p[1], 0u, 0x03u);
    dst_p->dsl_prtclt_fltr_actv_rgnrtion_status = unpack_right_shift_u8(src_p[1], 2u, 0x0cu);

    return (0);
}

uint8_t rmu_ctrl_canbus_data_dpfc1_dsl_prtclt_fltr_pssv_rgnrtion_status_encode(double value)
{
    return (uint8_t)(value);
}

double rmu_ctrl_canbus_data_dpfc1_dsl_prtclt_fltr_pssv_rgnrtion_status_decode(uint8_t value)
{
    return ((double)value);
}

bool rmu_ctrl_canbus_data_dpfc1_dsl_prtclt_fltr_pssv_rgnrtion_status_is_in_range(uint8_t value)
{
    return (value <= 3u);
}

uint8_t rmu_ctrl_canbus_data_dpfc1_dsl_prtclt_fltr_actv_rgnrtion_status_encode(double value)
{
    return (uint8_t)(value);
}

double rmu_ctrl_canbus_data_dpfc1_dsl_prtclt_fltr_actv_rgnrtion_status_decode(uint8_t value)
{
    return ((double)value);
}

bool rmu_ctrl_canbus_data_dpfc1_dsl_prtclt_fltr_actv_rgnrtion_status_is_in_range(uint8_t value)
{
    return (value <= 3u);
}

int rmu_ctrl_canbus_data_ic1_pack(
    uint8_t *dst_p,
    const struct rmu_ctrl_canbus_data_ic1_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[5] |= pack_left_shift_u16(src_p->eng_exhaust_gas_temp, 0u, 0xffu);
    dst_p[6] |= pack_right_shift_u16(src_p->eng_exhaust_gas_temp, 8u, 0xffu);

    return (8);
}

int rmu_ctrl_canbus_data_ic1_unpack(
    struct rmu_ctrl_canbus_data_ic1_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->eng_exhaust_gas_temp = unpack_right_shift_u16(src_p[5], 0u, 0xffu);
    dst_p->eng_exhaust_gas_temp |= unpack_left_shift_u16(src_p[6], 8u, 0xffu);

    return (0);
}

uint16_t rmu_ctrl_canbus_data_ic1_eng_exhaust_gas_temp_encode(double value)
{
    return (uint16_t)((value - -273.0) / 0.03125);
}

double rmu_ctrl_canbus_data_ic1_eng_exhaust_gas_temp_decode(uint16_t value)
{
    return (((double)value * 0.03125) + -273.0);
}

bool rmu_ctrl_canbus_data_ic1_eng_exhaust_gas_temp_is_in_range(uint16_t value)
{
    return (value <= 64255u);
}

int rmu_ctrl_canbus_data_lfe_pack(
    uint8_t *dst_p,
    const struct rmu_ctrl_canbus_data_lfe_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u16(src_p->eng_fuel_rate, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(src_p->eng_fuel_rate, 8u, 0xffu);

    return (8);
}

int rmu_ctrl_canbus_data_lfe_unpack(
    struct rmu_ctrl_canbus_data_lfe_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->eng_fuel_rate = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    dst_p->eng_fuel_rate |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);

    return (0);
}

uint16_t rmu_ctrl_canbus_data_lfe_eng_fuel_rate_encode(double value)
{
    return (uint16_t)(value / 0.05);
}

double rmu_ctrl_canbus_data_lfe_eng_fuel_rate_decode(uint16_t value)
{
    return ((double)value * 0.05);
}

bool rmu_ctrl_canbus_data_lfe_eng_fuel_rate_is_in_range(uint16_t value)
{
    return (value <= 64255u);
}

int rmu_ctrl_canbus_data_ei_pack(
    uint8_t *dst_p,
    const struct rmu_ctrl_canbus_data_ei_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[6] |= pack_left_shift_u16(src_p->instantaneous_est_brake_power, 0u, 0xffu);
    dst_p[7] |= pack_right_shift_u16(src_p->instantaneous_est_brake_power, 8u, 0xffu);

    return (8);
}

int rmu_ctrl_canbus_data_ei_unpack(
    struct rmu_ctrl_canbus_data_ei_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->instantaneous_est_brake_power = unpack_right_shift_u16(src_p[6], 0u, 0xffu);
    dst_p->instantaneous_est_brake_power |= unpack_left_shift_u16(src_p[7], 8u, 0xffu);

    return (0);
}

uint16_t rmu_ctrl_canbus_data_ei_instantaneous_est_brake_power_encode(double value)
{
    return (uint16_t)(value / 0.5);
}

double rmu_ctrl_canbus_data_ei_instantaneous_est_brake_power_decode(uint16_t value)
{
    return ((double)value * 0.5);
}

bool rmu_ctrl_canbus_data_ei_instantaneous_est_brake_power_is_in_range(uint16_t value)
{
    return (value <= 64255u);
}

int rmu_ctrl_canbus_data_egf1_pack(
    uint8_t *dst_p,
    const struct rmu_ctrl_canbus_data_egf1_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[2] |= pack_left_shift_u16(src_p->eng_intake_air_mass_flow_rate, 0u, 0xffu);
    dst_p[3] |= pack_right_shift_u16(src_p->eng_intake_air_mass_flow_rate, 8u, 0xffu);

    return (8);
}

int rmu_ctrl_canbus_data_egf1_unpack(
    struct rmu_ctrl_canbus_data_egf1_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->eng_intake_air_mass_flow_rate = unpack_right_shift_u16(src_p[2], 0u, 0xffu);
    dst_p->eng_intake_air_mass_flow_rate |= unpack_left_shift_u16(src_p[3], 8u, 0xffu);

    return (0);
}

uint16_t rmu_ctrl_canbus_data_egf1_eng_intake_air_mass_flow_rate_encode(double value)
{
    return (uint16_t)(value / 0.05);
}

double rmu_ctrl_canbus_data_egf1_eng_intake_air_mass_flow_rate_decode(uint16_t value)
{
    return ((double)value * 0.05);
}

bool rmu_ctrl_canbus_data_egf1_eng_intake_air_mass_flow_rate_is_in_range(uint16_t value)
{
    return (value <= 64255u);
}

int rmu_ctrl_canbus_data_eec3_pack(
    uint8_t *dst_p,
    const struct rmu_ctrl_canbus_data_eec3_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[5] |= pack_left_shift_u16(src_p->aftrtratment1_exhaust_gas_mass_flow, 0u, 0xffu);
    dst_p[6] |= pack_right_shift_u16(src_p->aftrtratment1_exhaust_gas_mass_flow, 8u, 0xffu);

    return (8);
}

int rmu_ctrl_canbus_data_eec3_unpack(
    struct rmu_ctrl_canbus_data_eec3_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->aftrtratment1_exhaust_gas_mass_flow = unpack_right_shift_u16(src_p[5], 0u, 0xffu);
    dst_p->aftrtratment1_exhaust_gas_mass_flow |= unpack_left_shift_u16(src_p[6], 8u, 0xffu);

    return (0);
}

uint16_t rmu_ctrl_canbus_data_eec3_aftrtratment1_exhaust_gas_mass_flow_encode(double value)
{
    return (uint16_t)(value / 0.2);
}

double rmu_ctrl_canbus_data_eec3_aftrtratment1_exhaust_gas_mass_flow_decode(uint16_t value)
{
    return ((double)value * 0.2);
}

bool rmu_ctrl_canbus_data_eec3_aftrtratment1_exhaust_gas_mass_flow_is_in_range(uint16_t value)
{
    return (value <= 64255u);
}

int rmu_ctrl_canbus_data_eec2_pack(
    uint8_t *dst_p,
    const struct rmu_ctrl_canbus_data_eec2_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[2] |= pack_left_shift_u8(src_p->eng_percent_load_at_current_speed, 0u, 0xffu);

    return (8);
}

int rmu_ctrl_canbus_data_eec2_unpack(
    struct rmu_ctrl_canbus_data_eec2_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->eng_percent_load_at_current_speed = unpack_right_shift_u8(src_p[2], 0u, 0xffu);

    return (0);
}

uint8_t rmu_ctrl_canbus_data_eec2_eng_percent_load_at_current_speed_encode(double value)
{
    return (uint8_t)(value);
}

double rmu_ctrl_canbus_data_eec2_eng_percent_load_at_current_speed_decode(uint8_t value)
{
    return ((double)value);
}

bool rmu_ctrl_canbus_data_eec2_eng_percent_load_at_current_speed_is_in_range(uint8_t value)
{
    return (value <= 250u);
}

int rmu_ctrl_canbus_data_eec1_pack(
    uint8_t *dst_p,
    const struct rmu_ctrl_canbus_data_eec1_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[2] |= pack_left_shift_u8(src_p->actual_eng_percent_torque, 0u, 0xffu);
    dst_p[3] |= pack_left_shift_u16(src_p->eng_speed, 0u, 0xffu);
    dst_p[4] |= pack_right_shift_u16(src_p->eng_speed, 8u, 0xffu);

    return (8);
}

int rmu_ctrl_canbus_data_eec1_unpack(
    struct rmu_ctrl_canbus_data_eec1_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->actual_eng_percent_torque = unpack_right_shift_u8(src_p[2], 0u, 0xffu);
    dst_p->eng_speed = unpack_right_shift_u16(src_p[3], 0u, 0xffu);
    dst_p->eng_speed |= unpack_left_shift_u16(src_p[4], 8u, 0xffu);

    return (0);
}

uint8_t rmu_ctrl_canbus_data_eec1_actual_eng_percent_torque_encode(double value)
{
    return (uint8_t)(value - -125.0);
}

double rmu_ctrl_canbus_data_eec1_actual_eng_percent_torque_decode(uint8_t value)
{
    return ((double)value + -125.0);
}

bool rmu_ctrl_canbus_data_eec1_actual_eng_percent_torque_is_in_range(uint8_t value)
{
    return (value <= 250u);
}

uint16_t rmu_ctrl_canbus_data_eec1_eng_speed_encode(double value)
{
    return (uint16_t)(value / 0.125);
}

double rmu_ctrl_canbus_data_eec1_eng_speed_decode(uint16_t value)
{
    return ((double)value * 0.125);
}

bool rmu_ctrl_canbus_data_eec1_eng_speed_is_in_range(uint16_t value)
{
    return (value <= 64255u);
}

int rmu_ctrl_canbus_data_ec1_pack(
    uint8_t *dst_p,
    const struct rmu_ctrl_canbus_data_ec1_t *src_p,
    size_t size)
{
    if (size < 39u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 39);

    dst_p[19] |= pack_left_shift_u16(src_p->eng_reference_torque, 0u, 0xffu);
    dst_p[20] |= pack_right_shift_u16(src_p->eng_reference_torque, 8u, 0xffu);

    return (39);
}

int rmu_ctrl_canbus_data_ec1_unpack(
    struct rmu_ctrl_canbus_data_ec1_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 39u) {
        return (-EINVAL);
    }

    dst_p->eng_reference_torque = unpack_right_shift_u16(src_p[19], 0u, 0xffu);
    dst_p->eng_reference_torque |= unpack_left_shift_u16(src_p[20], 8u, 0xffu);

    return (0);
}

uint16_t rmu_ctrl_canbus_data_ec1_eng_reference_torque_encode(double value)
{
    return (uint16_t)(value);
}

double rmu_ctrl_canbus_data_ec1_eng_reference_torque_decode(uint16_t value)
{
    return ((double)value);
}

bool rmu_ctrl_canbus_data_ec1_eng_reference_torque_is_in_range(uint16_t value)
{
    return (value <= 64255u);
}

int rmu_ctrl_canbus_data_ccvs_pack(
    uint8_t *dst_p,
    const struct rmu_ctrl_canbus_data_ccvs_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[1] |= pack_left_shift_u16(src_p->wheel_based_vehicle_speed, 0u, 0xffu);
    dst_p[2] |= pack_right_shift_u16(src_p->wheel_based_vehicle_speed, 8u, 0xffu);

    return (8);
}

int rmu_ctrl_canbus_data_ccvs_unpack(
    struct rmu_ctrl_canbus_data_ccvs_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->wheel_based_vehicle_speed = unpack_right_shift_u16(src_p[1], 0u, 0xffu);
    dst_p->wheel_based_vehicle_speed |= unpack_left_shift_u16(src_p[2], 8u, 0xffu);

    return (0);
}

uint16_t rmu_ctrl_canbus_data_ccvs_wheel_based_vehicle_speed_encode(double value)
{
    return (uint16_t)(value / 0.00390625);
}

double rmu_ctrl_canbus_data_ccvs_wheel_based_vehicle_speed_decode(uint16_t value)
{
    return ((double)value * 0.00390625);
}

bool rmu_ctrl_canbus_data_ccvs_wheel_based_vehicle_speed_is_in_range(uint16_t value)
{
    return (value <= 64254u);
}

int rmu_ctrl_canbus_data_at1_ig2_pack(
    uint8_t *dst_p,
    const struct rmu_ctrl_canbus_data_at1_ig2_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u16(src_p->aftertreatment1_exhaust_gas_temp1, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(src_p->aftertreatment1_exhaust_gas_temp1, 8u, 0xffu);

    return (8);
}

int rmu_ctrl_canbus_data_at1_ig2_unpack(
    struct rmu_ctrl_canbus_data_at1_ig2_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->aftertreatment1_exhaust_gas_temp1 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    dst_p->aftertreatment1_exhaust_gas_temp1 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);

    return (0);
}

uint16_t rmu_ctrl_canbus_data_at1_ig2_aftertreatment1_exhaust_gas_temp1_encode(double value)
{
    return (uint16_t)((value - -273.0) / 0.03125);
}

double rmu_ctrl_canbus_data_at1_ig2_aftertreatment1_exhaust_gas_temp1_decode(uint16_t value)
{
    return (((double)value * 0.03125) + -273.0);
}

bool rmu_ctrl_canbus_data_at1_ig2_aftertreatment1_exhaust_gas_temp1_is_in_range(uint16_t value)
{
    return (value <= 64255u);
}

int rmu_ctrl_canbus_data_at1_img_pack(
    uint8_t *dst_p,
    const struct rmu_ctrl_canbus_data_at1_img_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u16(src_p->aftertreatment1_exhaust_gas_temp2, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(src_p->aftertreatment1_exhaust_gas_temp2, 8u, 0xffu);

    return (8);
}

int rmu_ctrl_canbus_data_at1_img_unpack(
    struct rmu_ctrl_canbus_data_at1_img_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->aftertreatment1_exhaust_gas_temp2 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    dst_p->aftertreatment1_exhaust_gas_temp2 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);

    return (0);
}

uint16_t rmu_ctrl_canbus_data_at1_img_aftertreatment1_exhaust_gas_temp2_encode(double value)
{
    return (uint16_t)((value - -273.0) / 0.03125);
}

double rmu_ctrl_canbus_data_at1_img_aftertreatment1_exhaust_gas_temp2_decode(uint16_t value)
{
    return (((double)value * 0.03125) + -273.0);
}

bool rmu_ctrl_canbus_data_at1_img_aftertreatment1_exhaust_gas_temp2_is_in_range(uint16_t value)
{
    return (value <= 64255u);
}

int rmu_ctrl_canbus_data_at1_og2_pack(
    uint8_t *dst_p,
    const struct rmu_ctrl_canbus_data_at1_og2_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u16(src_p->aftertreatment1_exhaust_gas_temp3, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(src_p->aftertreatment1_exhaust_gas_temp3, 8u, 0xffu);

    return (8);
}

int rmu_ctrl_canbus_data_at1_og2_unpack(
    struct rmu_ctrl_canbus_data_at1_og2_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->aftertreatment1_exhaust_gas_temp3 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    dst_p->aftertreatment1_exhaust_gas_temp3 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);

    return (0);
}

uint16_t rmu_ctrl_canbus_data_at1_og2_aftertreatment1_exhaust_gas_temp3_encode(double value)
{
    return (uint16_t)((value - -273.0) / 0.03125);
}

double rmu_ctrl_canbus_data_at1_og2_aftertreatment1_exhaust_gas_temp3_decode(uint16_t value)
{
    return (((double)value * 0.03125) + -273.0);
}

bool rmu_ctrl_canbus_data_at1_og2_aftertreatment1_exhaust_gas_temp3_is_in_range(uint16_t value)
{
    return (value <= 64255u);
}

int rmu_ctrl_canbus_data_egt_data_pack(
    uint8_t *dst_p,
    const struct rmu_ctrl_canbus_data_egt_data_t *src_p,
    size_t size)
{
    uint32_t egt_data_0;
    uint32_t egt_data_1;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    egt_data_0 = (uint32_t)src_p->egt_data_0;
    dst_p[0] |= pack_right_shift_u32(egt_data_0, 24u, 0xffu);
    dst_p[1] |= pack_right_shift_u32(egt_data_0, 16u, 0xffu);
    dst_p[2] |= pack_right_shift_u32(egt_data_0, 8u, 0xffu);
    dst_p[3] |= pack_left_shift_u32(egt_data_0, 0u, 0xffu);
    egt_data_1 = (uint32_t)src_p->egt_data_1;
    dst_p[4] |= pack_right_shift_u32(egt_data_1, 24u, 0xffu);
    dst_p[5] |= pack_right_shift_u32(egt_data_1, 16u, 0xffu);
    dst_p[6] |= pack_right_shift_u32(egt_data_1, 8u, 0xffu);
    dst_p[7] |= pack_left_shift_u32(egt_data_1, 0u, 0xffu);

    return (8);
}

int rmu_ctrl_canbus_data_egt_data_unpack(
    struct rmu_ctrl_canbus_data_egt_data_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint32_t egt_data_0;
    uint32_t egt_data_1;

    if (size < 8u) {
        return (-EINVAL);
    }

    egt_data_0 = unpack_left_shift_u32(src_p[0], 24u, 0xffu);
    egt_data_0 |= unpack_left_shift_u32(src_p[1], 16u, 0xffu);
    egt_data_0 |= unpack_left_shift_u32(src_p[2], 8u, 0xffu);
    egt_data_0 |= unpack_right_shift_u32(src_p[3], 0u, 0xffu);
    dst_p->egt_data_0 = (int32_t)egt_data_0;
    egt_data_1 = unpack_left_shift_u32(src_p[4], 24u, 0xffu);
    egt_data_1 |= unpack_left_shift_u32(src_p[5], 16u, 0xffu);
    egt_data_1 |= unpack_left_shift_u32(src_p[6], 8u, 0xffu);
    egt_data_1 |= unpack_right_shift_u32(src_p[7], 0u, 0xffu);
    dst_p->egt_data_1 = (int32_t)egt_data_1;

    return (0);
}

int32_t rmu_ctrl_canbus_data_egt_data_egt_data_0_encode(double value)
{
    return (int32_t)(value / 0.01);
}

double rmu_ctrl_canbus_data_egt_data_egt_data_0_decode(int32_t value)
{
    return ((double)value * 0.01);
}

bool rmu_ctrl_canbus_data_egt_data_egt_data_0_is_in_range(int32_t value)
{
    return ((value >= -50000) && (value <= 120000));
}

int32_t rmu_ctrl_canbus_data_egt_data_egt_data_1_encode(double value)
{
    return (int32_t)(value / 0.01);
}

double rmu_ctrl_canbus_data_egt_data_egt_data_1_decode(int32_t value)
{
    return ((double)value * 0.01);
}

bool rmu_ctrl_canbus_data_egt_data_egt_data_1_is_in_range(int32_t value)
{
    return ((value >= -50000) && (value <= 120000));
}

int rmu_ctrl_canbus_data_egt_data_0_pack(
    uint8_t *dst_p,
    const struct rmu_ctrl_canbus_data_egt_data_0_t *src_p,
    size_t size)
{
    uint32_t egt_0;
    uint32_t egt_1;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    egt_0 = (uint32_t)src_p->egt_0;
    dst_p[0] |= pack_right_shift_u32(egt_0, 24u, 0xffu);
    dst_p[1] |= pack_right_shift_u32(egt_0, 16u, 0xffu);
    dst_p[2] |= pack_right_shift_u32(egt_0, 8u, 0xffu);
    dst_p[3] |= pack_left_shift_u32(egt_0, 0u, 0xffu);
    egt_1 = (uint32_t)src_p->egt_1;
    dst_p[4] |= pack_right_shift_u32(egt_1, 24u, 0xffu);
    dst_p[5] |= pack_right_shift_u32(egt_1, 16u, 0xffu);
    dst_p[6] |= pack_right_shift_u32(egt_1, 8u, 0xffu);
    dst_p[7] |= pack_left_shift_u32(egt_1, 0u, 0xffu);

    return (8);
}

int rmu_ctrl_canbus_data_egt_data_0_unpack(
    struct rmu_ctrl_canbus_data_egt_data_0_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint32_t egt_0;
    uint32_t egt_1;

    if (size < 8u) {
        return (-EINVAL);
    }

    egt_0 = unpack_left_shift_u32(src_p[0], 24u, 0xffu);
    egt_0 |= unpack_left_shift_u32(src_p[1], 16u, 0xffu);
    egt_0 |= unpack_left_shift_u32(src_p[2], 8u, 0xffu);
    egt_0 |= unpack_right_shift_u32(src_p[3], 0u, 0xffu);
    dst_p->egt_0 = (int32_t)egt_0;
    egt_1 = unpack_left_shift_u32(src_p[4], 24u, 0xffu);
    egt_1 |= unpack_left_shift_u32(src_p[5], 16u, 0xffu);
    egt_1 |= unpack_left_shift_u32(src_p[6], 8u, 0xffu);
    egt_1 |= unpack_right_shift_u32(src_p[7], 0u, 0xffu);
    dst_p->egt_1 = (int32_t)egt_1;

    return (0);
}

int32_t rmu_ctrl_canbus_data_egt_data_0_egt_0_encode(double value)
{
    return (int32_t)(value / 0.01);
}

double rmu_ctrl_canbus_data_egt_data_0_egt_0_decode(int32_t value)
{
    return ((double)value * 0.01);
}

bool rmu_ctrl_canbus_data_egt_data_0_egt_0_is_in_range(int32_t value)
{
    return ((value >= -50000) && (value <= 120000));
}

int32_t rmu_ctrl_canbus_data_egt_data_0_egt_1_encode(double value)
{
    return (int32_t)(value / 0.01);
}

double rmu_ctrl_canbus_data_egt_data_0_egt_1_decode(int32_t value)
{
    return ((double)value * 0.01);
}

bool rmu_ctrl_canbus_data_egt_data_0_egt_1_is_in_range(int32_t value)
{
    return ((value >= -50000) && (value <= 120000));
}

int rmu_ctrl_canbus_data_egt_data_1_pack(
    uint8_t *dst_p,
    const struct rmu_ctrl_canbus_data_egt_data_1_t *src_p,
    size_t size)
{
    uint32_t egt_2;
    uint32_t egt_3;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    egt_2 = (uint32_t)src_p->egt_2;
    dst_p[0] |= pack_right_shift_u32(egt_2, 24u, 0xffu);
    dst_p[1] |= pack_right_shift_u32(egt_2, 16u, 0xffu);
    dst_p[2] |= pack_right_shift_u32(egt_2, 8u, 0xffu);
    dst_p[3] |= pack_left_shift_u32(egt_2, 0u, 0xffu);
    egt_3 = (uint32_t)src_p->egt_3;
    dst_p[4] |= pack_right_shift_u32(egt_3, 24u, 0xffu);
    dst_p[5] |= pack_right_shift_u32(egt_3, 16u, 0xffu);
    dst_p[6] |= pack_right_shift_u32(egt_3, 8u, 0xffu);
    dst_p[7] |= pack_left_shift_u32(egt_3, 0u, 0xffu);

    return (8);
}

int rmu_ctrl_canbus_data_egt_data_1_unpack(
    struct rmu_ctrl_canbus_data_egt_data_1_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint32_t egt_2;
    uint32_t egt_3;

    if (size < 8u) {
        return (-EINVAL);
    }

    egt_2 = unpack_left_shift_u32(src_p[0], 24u, 0xffu);
    egt_2 |= unpack_left_shift_u32(src_p[1], 16u, 0xffu);
    egt_2 |= unpack_left_shift_u32(src_p[2], 8u, 0xffu);
    egt_2 |= unpack_right_shift_u32(src_p[3], 0u, 0xffu);
    dst_p->egt_2 = (int32_t)egt_2;
    egt_3 = unpack_left_shift_u32(src_p[4], 24u, 0xffu);
    egt_3 |= unpack_left_shift_u32(src_p[5], 16u, 0xffu);
    egt_3 |= unpack_left_shift_u32(src_p[6], 8u, 0xffu);
    egt_3 |= unpack_right_shift_u32(src_p[7], 0u, 0xffu);
    dst_p->egt_3 = (int32_t)egt_3;

    return (0);
}

int32_t rmu_ctrl_canbus_data_egt_data_1_egt_2_encode(double value)
{
    return (int32_t)(value / 0.01);
}

double rmu_ctrl_canbus_data_egt_data_1_egt_2_decode(int32_t value)
{
    return ((double)value * 0.01);
}

bool rmu_ctrl_canbus_data_egt_data_1_egt_2_is_in_range(int32_t value)
{
    return ((value >= -50000) && (value <= 120000));
}

int32_t rmu_ctrl_canbus_data_egt_data_1_egt_3_encode(double value)
{
    return (int32_t)(value / 0.01);
}

double rmu_ctrl_canbus_data_egt_data_1_egt_3_decode(int32_t value)
{
    return ((double)value * 0.01);
}

bool rmu_ctrl_canbus_data_egt_data_1_egt_3_is_in_range(int32_t value)
{
    return ((value >= -50000) && (value <= 120000));
}
