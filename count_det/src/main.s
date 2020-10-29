.text
.global count_det 
count_det:
    // Init registers
    // In r1 load memory from r0
    // In r2 carry multiplication
    // In r3 carry answer

    mov r1, #0
    mov r2, #0
    mov r3, #0

    // result += a[0] * a[4] * a[8]
    ldr r2, [r0]
    ldr r1, [r0, #16]
    mul r2, r2, r1
    ldr r1, [r0, #32]
    mul r2, r2, r1
    add r3, r3, r2

    // result += a[6] * a[1] * a[5]
    ldr r2, [r0, #24]
    ldr r1, [r0, #4]
    mul r2, r2, r1
    ldr r1, [r0, #20]
    mul r2, r2, r1
    add r3, r3, r2

    // result += a[3] * a[7] * a[2]
    ldr r2, [r0, #12]
    ldr r1, [r0, #28]
    mul r2, r2, r1
    ldr r1, [r0, #8]
    mul r2, r2, r1
    add r3, r3, r2

    // result -= a[6] * a[4] * a[2]
    ldr r2, [r0, #24]
    ldr r1, [r0, #16]
    mul r2, r2, r1
    ldr r1, [r0, #8]
    mul r2, r2, r1
    sub r3, r3, r2

    // result -= a[0] * a[7] * a[5]
    ldr r2, [r0]
    ldr r1, [r0, #28]
    mul r2, r2, r1
    ldr r1, [r0, #20]
    mul r2, r2, r1
    sub r3, r3, r2

    // result -= a[3] * a[1] * a[8]
    ldr r2, [r0, #12]
    ldr r1, [r0, #4]
    mul r2, r2, r1
    ldr r1, [r0, #32]
    mul r2, r2, r1
    sub r3, r3, r2

    mov r0, r3

    bx lr
