; Copyright 2025 pugur
; This source code is licensed under the Apache License, Version 2.0
; which can be found in the LICENSE file.

section .text

; export
global is_ascii_letter_x86
global is_ascii_digit_x86
global is_ascii_alnum_x86
global is_ascii_letters_bulk_x86
global is_ascii_digits_bulk_x86

; function: is_ascii_letter_x86
; parameters:
;   rdi - char32_t codepoint
; returns: bool (1 = true, 0 = false)
is_ascii_letter_x86:
    ; check if codepoint > 0x7F (not ascii)
    cmp     edi, 0x7F
    ja      .not_letter

    ; check uppercase A-Z (0x41-0x5A)
    mov     eax, edi
    sub     eax, 'A'        ; eax = codepoint - 'A'
    cmp     eax, 25         ; compare with 'Z' - 'A'
    jbe     .is_letter      ; if <= 25, it's uppercase

    ; check lowercase a-z (0x61-0x7A)
    mov     eax, edi
    sub     eax, 'a'        ; eax = codepoint - 'a'
    cmp     eax, 25         ; compare with 'z' - 'a'
    jbe     .is_letter      ; if <= 25, it's lowercase

.not_letter:
    xor     eax, eax        ; return false
    ret

.is_letter:
    mov     eax, 1          ; return true
    ret

; function: is_ascii_digit_x86
; parameters:
;   rdi - char32_t codepoint
; returns: bool (1 = true, 0 = false)
is_ascii_digit_x86:
    ; check if codepoint > 0x7F (not ascii)
    cmp     edi, 0x7F
    ja      .not_digit

    ; check digits 0-9 (0x30-0x39)
    mov     eax, edi
    sub     eax, '0'        ; eax = codepoint - '0'
    cmp     eax, 9          ; compare with '9' - '0'
    jbe     .is_digit       ; if <= 9, it's a digit

.not_digit:
    xor     eax, eax        ; return false
    ret

.is_digit:
    mov     eax, 1          ; return true
    ret

; function: is_ascii_alnum_x86
; parameters:
;   rdi - char32_t codepoint
; returns: bool (1 = true, 0 = false)
is_ascii_alnum_x86:
    ; check if codepoint > 0x7F (not ascii)
    cmp     edi, 0x7F
    ja      .not_alnum

    ; check digits 0-9 first (most common)
    mov     eax, edi
    sub     eax, '0'
    cmp     eax, 9
    jbe     .is_alnum

    ; check uppercase A-Z
    mov     eax, edi
    sub     eax, 'A'
    cmp     eax, 25
    jbe     .is_alnum

    ; check lowercase a-z
    mov     eax, edi
    sub     eax, 'a'
    cmp     eax, 25
    jbe     .is_alnum

.not_alnum:
    xor     eax, eax        ; return false
    ret

.is_alnum:
    mov     eax, 1          ; return true
    ret

; function: is_ascii_letters_bulk_x86
; parameters:
;   rdi - const char32_t* codepoints
;   rsi - bool* results
;   rdx - std::size_t count
; returns: void
is_ascii_letters_bulk_x86:
    test    rdx, rdx        ; check if count == 0
    jz      .done

    push    rbx
    push    r12
    push    r13

    mov     r12, rdi        ; r12 = codepoints
    mov     r13, rsi        ; r13 = results
    xor     rbx, rbx        ; rbx = index

.loop:
    mov     edi, [r12 + rbx*4]  ; load codepoint

    ; check if ascii
    cmp     edi, 0x7F
    ja      .not_letter_bulk

    ; check uppercase A-Z
    mov     eax, edi
    sub     eax, 'A'
    cmp     eax, 25
    jbe     .is_letter_bulk

    ; check lowercase a-z
    mov     eax, edi
    sub     eax, 'a'
    cmp     eax, 25
    jbe     .is_letter_bulk

.not_letter_bulk:
    mov     byte [r13 + rbx], 0     ; results[i] = false
    jmp     .next

.is_letter_bulk:
    mov     byte [r13 + rbx], 1     ; results[i] = true

.next:
    inc     rbx
    cmp     rbx, rdx
    jb      .loop

    pop     r13
    pop     r12
    pop     rbx

.done:
    ret

; function: is_ascii_digits_bulk_x86
; parameters:
;   rdi - const char32_t* codepoints
;   rsi - bool* results
;   rdx - std::size_t count
; returns: void
is_ascii_digits_bulk_x86:
    test    rdx, rdx
    jz      .done_digits

    push    rbx
    push    r12
    push    r13

    mov     r12, rdi        ; r12 = codepoints
    mov     r13, rsi        ; r13 = results
    xor     rbx, rbx        ; rbx = index

.loop_digits:
    mov     edi, [r12 + rbx*4]  ; load codepoint

    ; check if ascii
    cmp     edi, 0x7F
    ja      .not_digit_bulk

    ; check digits 0-9
    mov     eax, edi
    sub     eax, '0'
    cmp     eax, 9
    jbe     .is_digit_bulk

.not_digit_bulk:
    mov     byte [r13 + rbx], 0     ; results[i] = false
    jmp     .next_digits

.is_digit_bulk:
    mov     byte [r13 + rbx], 1     ; results[i] = true

.next_digits:
    inc     rbx
    cmp     rbx, rdx
    jb      .loop_digits

    pop     r13
    pop     r12
    pop     rbx

.done_digits:
    ret
