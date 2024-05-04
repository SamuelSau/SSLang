; ModuleID = 'MyModule'
source_filename = "MyModule"

@q = global float 0x4029E05C00000000, align 4
@r = global float 0x41AB909F60000000, align 4
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
@formalGreeting = private constant [6 x i8] c"hello\00", align 1
@printedFormatBool = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@trueStr = private unnamed_addr constant [5 x i8] c"true\00", align 1
@falseStr = private unnamed_addr constant [6 x i8] c"false\00", align 1
@printedFloatInt = private unnamed_addr constant [4 x i8] c"%f\0A\00", align 1
@printedFormatInt = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

define i32 @returnValidString() {
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
  %loopcond = icmp slt i32 %loopVar1, 2
  br i1 %loopcond, label %forBody, label %forEnd

forBody:                                          ; preds = %forCond
  %t = load i32, ptr @t, align 4
  %addtmp = add i32 %t, 1
  store i32 %addtmp, ptr @t, align 4
  %isDeclared = load i1, ptr @isDeclared, align 1
  %0 = select i1 %isDeclared, ptr @trueStr, ptr @falseStr
  %1 = call i32 (ptr, ...) @printf(ptr @printedFormatBool, ptr %0)
  %q = load float, ptr @q, align 4
  %floatToDouble = fpext float %q to double
  %2 = call i32 (ptr, ...) @printf(ptr @printedFloatInt, double %floatToDouble)
  %nextVar = add i32 %loopVar1, 1
  store i32 %nextVar, ptr %loopVar, align 4
  br label %forCond

forEnd:                                           ; preds = %forCond
  %y = load i32, ptr @y, align 4
  ret i32 %y

whileCond:                                        ; preds = %whileBody, %else
  %x2 = load i32, ptr @x, align 4
  %cmptmp3 = icmp sle i32 %x2, 10
  br i1 %cmptmp3, label %whileBody, label %whileExit

whileBody:                                        ; preds = %whileCond
  %x4 = load i32, ptr @x, align 4
  %3 = call i32 (ptr, ...) @printf(ptr @printedFormatInt, i32 %x4)
  %x5 = load i32, ptr @x, align 4
  %addtmp6 = add i32 %x5, 1
  store i32 %addtmp6, ptr @x, align 4
  br label %whileCond

whileExit:                                        ; preds = %whileCond
  %x7 = load i32, ptr @x, align 4
  ret i32 %x7
}

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %0 = call i32 @returnValidString()
  ret i32 0
}
