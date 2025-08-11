; Copyright 2025 pugur
; This source code is licensed under the Apache License, Version 2.0
; which can be found in the LICENSE file.

section .text

; export
global is_ascii_letters_bulk_avx2_x86
global is_ascii_digits_bulk_avx2_x86

; function: is_ascii_letters_bulk_avx2_x86
; parameters:
;   rdi - const char32_t* codepoints
;   rsi - bool* results
;   rdx - std::size_t count
; returns: bool (always true in rax)
is_ascii_letters_bulk_avx2_x86:
    test    rdx, rdx
    jz      .done

    push    rbx
    push    r12
    push    r13

    mov     r12, rdi        ; r12 = codepoints
    mov     r13, rsi        ; r13 = results
    mov     rbx, rdx        ; rbx = count

    mov     rax, rbx
    and     rax, ~7
    mov     r8, rax         ; r8 = simd_count

    ; prepare AVX2 constants
    mov     eax, 0x7F
    vmovd   xmm0, eax
    vpbroadcastd ymm1, xmm0     ; ascii_max

    mov     eax, 'A'
    vmovd   xmm0, eax
    vpbroadcastd ymm2, xmm0     ; upper_min

    mov     eax, 'Z'
    vmovd   xmm0, eax
    vpbroadcastd ymm3, xmm0     ; upper_max

    mov     eax, 'a'
    vmovd   xmm0, eax
    vpbroadcastd ymm4, xmm0     ; lower_min

    mov     eax, 'z'
    vmovd   xmm0, eax
    vpbroadcastd ymm5, xmm0     ; lower_max

    mov     eax, 1
    vmovd   xmm0, eax
    vpbroadcastd ymm6, xmm0     ; constant 1

    xor     rcx, rcx            ; index = 0
.simd_loop:
    cmp     rcx, r8
    jge     .scalar_loop

    vmovdqu ymm0, [r12 + rcx*4] ; load 8 codepoints

    vpaddd  ymm7, ymm1, ymm6    ; ascii_max + 1
    vpcmpgtd ymm8, ymm7, ymm0   ; is_ascii

    vpaddd  ymm7, ymm0, ymm6    ; chars + 1
    vpcmpgtd ymm9, ymm7, ymm2   ; chars + 1 > upper_min
    vpaddd  ymm7, ymm3, ymm6    ; upper_max + 1
    vpcmpgtd ymm10, ymm7, ymm0  ; upper_max + 1 > chars
    vpand   ymm9, ymm9, ymm10   ; is_upper

    vpaddd  ymm7, ymm0, ymm6    ; chars + 1
    vpcmpgtd ymm10, ymm7, ymm4  ; chars + 1 > lower_min
    vpaddd  ymm7, ymm5, ymm6    ; lower_max + 1
    vpcmpgtd ymm11, ymm7, ymm0  ; lower_max + 1 > chars
    vpand   ymm10, ymm10, ymm11 ; is_lower

    vpor    ymm9, ymm9, ymm10   ; is_upper || is_lower
    vpand   ymm8, ymm8, ymm9    ; is_letter

    sub     rsp, 32
    vmovdqa [rsp], ymm8

    mov     r9, 0
.convert_simd:
    cmp     r9, 8
    jge     .convert_simd_done

    mov     eax, [rsp + r9*4]
    test    eax, eax
    setne   al
    mov r10, rcx
    add r10, r9
    mov [r13 + r10], al

    inc     r9
    jmp     .convert_simd

.convert_simd_done:
    add     rsp, 32
    add     rcx, 8
    jmp     .simd_loop

.scalar_loop:
    cmp     rcx, rbx
    jge     .done

    mov     eax, [r12 + rcx*4]      ; load codepoint

    cmp     eax, 0x7F
    ja      .not_letter_scalar

    cmp     eax, 'A'
    jb      .not_letter_scalar
    cmp     eax, 'Z'
    jbe     .is_letter_scalar
    cmp     eax, 'a'
    jb      .not_letter_scalar
    cmp     eax, 'z'
    jbe     .is_letter_scalar

.not_letter_scalar:
    mov     byte [r13 + rcx], 0
    jmp     .next_scalar

.is_letter_scalar:
    mov     byte [r13 + rcx], 1

.next_scalar:
    inc     rcx
    jmp     .scalar_loop

.done:
    vzeroupper
    mov     rax, 1

    pop     r13
    pop     r12
    pop     rbx
    ret

; function: is_ascii_digits_bulk_avx2_x86
; parameters:
;   rdi - const char32_t* codepoints
;   rsi - bool* results
;   rdx - std::size_t count
; returns: bool (always true in rax)
is_ascii_digits_bulk_avx2_x86:
    test    rdx, rdx
    jz      .done_digits

    push    rbx
    push    r12
    push    r13

    mov     r12, rdi        ; r12 = codepoints
    mov     r13, rsi        ; r13 = results
    mov     rbx, rdx        ; rbx = count

    mov     rax, rbx
    and     rax, ~7
    mov     r8, rax         ; r8 = simd_count

    mov     eax, 0x7F
    vmovd   xmm0, eax
    vpbroadcastd ymm1, xmm0     ; ascii_max

    mov     eax, '0'
    vmovd   xmm0, eax
    vpbroadcastd ymm2, xmm0     ; digit_min

    mov     eax, '9'
    vmovd   xmm0, eax
    vpbroadcastd ymm3, xmm0     ; digit_max

    mov     eax, 1
    vmovd   xmm0, eax
    vpbroadcastd ymm4, xmm0     ; constant 1

    xor     rcx, rcx            ; index = 0
.simd_loop_digits:
    cmp     rcx, r8
    jge     .scalar_loop_digits

    vmovdqu ymm0, [r12 + rcx*4] ; load 8 codepoints

    vpaddd  ymm5, ymm1, ymm4    ; ascii_max + 1
    vpcmpgtd ymm6, ymm5, ymm0   ; is_ascii

    vpaddd  ymm5, ymm0, ymm4    ; chars + 1
    vpcmpgtd ymm7, ymm5, ymm2   ; chars + 1 > digit_min
    vpaddd  ymm5, ymm3, ymm4    ; digit_max + 1
    vpcmpgtd ymm8, ymm5, ymm0   ; digit_max + 1 > chars
    vpand   ymm7, ymm7, ymm8    ; is_digit

    vpand   ymm6, ymm6, ymm7    ; result

    sub     rsp, 32
    vmovdqa [rsp], ymm6

    mov     r9, 0
.convert_simd_digits:
    cmp     r9, 8
    jge     .convert_simd_digits_done

    mov     eax, [rsp + r9*4]
    test    eax, eax
    setne   al
    mov r10, rcx
    add r10, r9
    mov [r13 + r10], al

    inc     r9
    jmp     .convert_simd_digits

.convert_simd_digits_done:
    add     rsp, 32
    add     rcx, 8
    jmp     .simd_loop_digits

.scalar_loop_digits:
    cmp     rcx, rbx
    jge     .done_digits

    mov     eax, [r12 + rcx*4]      ; load codepoint

    cmp     eax, 0x7F
    ja      .not_digit_scalar

    cmp     eax, '0'
    jb      .not_digit_scalar
    cmp     eax, '9'
    jbe     .is_digit_scalar

.not_digit_scalar:
    mov     byte [r13 + rcx], 0
    jmp     .next_scalar_digits

.is_digit_scalar:
    mov     byte [r13 + rcx], 1

.next_scalar_digits:
    inc     rcx
    jmp     .scalar_loop_digits

.done_digits:
    vzeroupper
    mov     rax, 1

    pop     r13
    pop     r12
    pop     rbx
    ret
