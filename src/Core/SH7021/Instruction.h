#pragma once

#include "default.h"

typedef union {
    struct {
        u16 lo: 8;
        u16 m: 4;
        u16 hi: 4;
    } m;

    struct {
        u16 lo: 4;
        u16 m: 4;
        u16 n: 4;
        u16 hi: 4;
    } nm;

    struct {
        u16 d: 4;
        u16 m: 4;
        u16 hi: 8;
    } md;

    struct {
        u16 d: 4;
        u16 n: 4;
        u16 hi: 8;
    } nd4;

    struct {
        u16 d: 4;
        u16 m: 4;
        u16 n: 4;
        u16 hi: 4;
    } nmd;

    struct {
        u16 d: 8;
        u16 hi: 8;
    } d;

    struct {
        u16 d: 12;
        u16 hi: 4;
    } d12;

    struct {
        u16 d: 8;
        u16 n: 4;
        u16 hi: 4;
    } nd8;

    struct {
        u16 i: 8;
        u16 hi: 8;
    } i;

    struct {
        u16 i: 8;
        u16 n: 4;
        u16 hi: 4;
    } ni;

    u16 raw;
} s_instruction;
