; ModuleID = 'MyModule'
source_filename = "MyModule"

@q = global float 0x3FFF02E060000000, align 4
@isDeclared = global i1 false, align 1
@x = global i32 2, align 4
@y = global i32 3, align 4
@p = global i32 7, align 4
@Q = global i32 9, align 4
@z = global i32 0, align 4
@t = global i32 0, align 4
@bb = global i32 100, align 4
@cc = global i32 10000000, align 4
@informalGreeting = private constant [4 x i8] c"sup\00", align 1
@printedFormatInt = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@printedFormatInt.1 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

define i32 @returnValidNumber() {
entry:
  %loopVar = alloca i32, align 4
  %x = load i32, ptr @x, align 4
  %cmptmp = icmp eq i32 %x, 2
  br i1 %cmptmp, label %then, label %else

then:                                             ; preds = %entry
  store i32 1, ptr %loopVar, align 4
  br label %forCond

else:                                             ; preds = %entry
  br label %whileCond

forCond:                                          ; preds = %forBody, %then
  %loopVar1 = load i32, ptr %loopVar, align 4
  %loopcond = icmp slt i32 %loopVar1, 10000000
  br i1 %loopcond, label %forBody, label %forEnd

forBody:                                          ; preds = %forCond
  %t = load i32, ptr @t, align 4
  %addtmp = add i32 %t, 1
  store i32 %addtmp, ptr @t, align 4
  %t2 = load i32, ptr @t, align 4
  %0 = call i32 (ptr, ...) @printf(ptr @printedFormatInt, i32 %t2)
  %nextVar = add i32 %loopVar1, 1
  store i32 %nextVar, ptr %loopVar, align 4
  br label %forCond

forEnd:                                           ; preds = %forCond
  %Q = load i32, ptr @Q, align 4
  ret i32 %Q

whileCond:                                        ; preds = %whileBody, %else
  %x3 = load i32, ptr @x, align 4
  %cmptmp4 = icmp sle i32 %x3, 10
  br i1 %cmptmp4, label %whileBody, label %whileExit

whileBody:                                        ; preds = %whileCond
  %x5 = load i32, ptr @x, align 4
  %1 = call i32 (ptr, ...) @printf(ptr @printedFormatInt.1, i32 %x5)
  %x6 = load i32, ptr @x, align 4
  %addtmp7 = add i32 %x6, 1
  store i32 %addtmp7, ptr @x, align 4
  br label %whileCond

whileExit:                                        ; preds = %whileCond
  %y = load i32, ptr @y, align 4
  ret i32 %y
}

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %0 = call i32 @returnValidNumber()
  ret i32 0
}
