; ModuleID = 'MyModule'
source_filename = "MyModule"

@q = global float 0x3FD3333340000000, align 4
@isDeclared = global i1 false, align 1
@x = global i32 2, align 4
@y = global i32 3, align 4
@p = global i32 7, align 4
@Q = global i32 9, align 4
@z = global i32 0, align 4
@t = global i32 2, align 4
@bb = global i32 100, align 4
@informalGreeting = private constant [4 x i8] c"sup\00", align 1
@printedFormatInt = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@printFormatInt = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@printedFormatInt.1 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@printFormatInt.2 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@printedFormatInt.3 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@printFormatInt.4 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

define i32 @main() {
entry:
}

define i32 @add() {
entry:
  %loopVar = alloca i32, align 4
  %bb = load i32, ptr @bb, align 4
  %0 = call i32 (ptr, ...) @printf(ptr @printFormatInt, i32 %bb)
  %x = load i32, ptr @x, align 4
  %cmptmp = icmp eq i32 %x, 2
  br i1 %cmptmp, label %then, label %else

then:                                             ; preds = %entry
  store i32 2, ptr %loopVar, align 4
  br label %forCond

else:                                             ; preds = %entry
  br label %whileCond

forCond:                                          ; preds = %forBody, %then
  %loopVar1 = load i32, ptr %loopVar, align 4
  %loopcond = icmp slt i32 %loopVar1, 3
  br i1 %loopcond, label %forBody, label %forEnd

forBody:                                          ; preds = %forCond
  %t = load i32, ptr @t, align 4
  %1 = call i32 (ptr, ...) @printf(ptr @printFormatInt.2, i32 %t)
  %nextVar = add i32 %loopVar1, 1
  store i32 %nextVar, ptr %loopVar, align 4
  br label %forCond

forEnd:                                           ; preds = %forCond
  %Q = load i32, ptr @Q, align 4
  ret i32 %Q

whileCond:                                        ; preds = %whileBody, %else
  %x2 = load i32, ptr @x, align 4
  %cmptmp3 = icmp slt i32 %x2, 10
  br i1 %cmptmp3, label %whileBody, label %whileExit

whileBody:                                        ; preds = %whileCond
  %x4 = load i32, ptr @x, align 4
  %2 = call i32 (ptr, ...) @printf(ptr @printFormatInt.4, i32 %x4)
  br label %whileCond

whileExit:                                        ; preds = %whileCond
  %y = load i32, ptr @y, align 4
  ret i32 %y
}

declare i32 @printf(ptr, ...)
