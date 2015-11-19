.syntax unified

.global fibonacci
fibonacci:
    push {r3, r4, r5, r6, r7, lr}

    cmp r0, 0
    ble .fib_L3

    cmp r0, 2 
    ble .fib_L4 
    
    mov r3, 0
    mov r4, 1
    mov r5, 1
    mov r7, 3
.fib_FOR:
    mov  r6, r4
    adds r4, r4, r3
    mov  r3, r6
    adds r5, r3, r4
    adds r7, r7, 1        
    cmp  r7, r0
    ble .fib_FOR
    
    mov r0, r5
    pop {r3, r4, r5, r6, r7, pc} 

.fib_L3:
    mov r0, 0
    pop {r3, r4, r5, r6, r7, pc} 

.fib_L4:
    mov r0, 1
    pop {r3, r4, r5, r6, r7, pc} 
