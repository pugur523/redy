# Operator Precedence

| Precedence | Operator                                        | Description                         | Associativity     |
| :--------: | :---------------------------------------------- | :---------------------------------- | :---------------- |
|     1      | `a++` `a--`                                     | postfix increment / decrement       | → (left-to-right) |
|     2      | `++a` `--a`                                     | prefix increment / decrement        | ← (right-to-left) |
|     2      | `!a` `~a`                                       | logical NOT / bitwise NOT           | ← (right-to-left) |
|     2      | `+a` `-a`                                       | unary plus / minus                  | ← (right-to-left) |
|     3      | `a ** b`                                        | exponentiation                      | ← (right-to-left) |
|     4      | `a * b` `a / b` `a % b`                         | multiplication, division, modulo    | → (left-to-right) |
|     5      | `a + b` `a - b`                                 | addition, subtraction               | → (left-to-right) |
|     6      | `a << b` `a >> b`                               | bitwise shift left / right          | → (left-to-right) |
|     7      | `a <=> b`                                       | three-way comparison                | → (left-to-right) |
|     8      | `a < b` `a <= b` `a > b` `a >= b`               | relational comparisons              | → (left-to-right) |
|     9      | `a == b` `a != b`                               | equality comparisons                | → (left-to-right) |
|     10     | `a & b`                                         | bitwise AND                         | → (left-to-right) |
|     11     | `a ^ b`                                         | bitwise XOR                         | → (left-to-right) |
|     12     | `a \| b`                                        | bitwise OR                          | → (left-to-right) |
|     13     | `a && b`                                        | logical AND                         | → (left-to-right) |
|     14     | `a \|\| b`                                      | logical OR                          | → (left-to-right) |
|     15     | `a = b` `a := b`                                | assignment (standard / declaration) | ← (right-to-left) |
|     15     | `a += b` `a -= b` `a *= b` `a /= b` `a %= b`    | compound assignment                 | ← (right-to-left) |
|     15     | `a &= b` `a ^= b` `a \|= b` `a <<= b` `a >>= b` | bitwise compound assignment         | ← (right-to-left) |
