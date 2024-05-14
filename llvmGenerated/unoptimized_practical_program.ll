; ModuleID = 'MyModule'
source_filename = "MyModule"

@x = global i32 10, align 4
@y = global i32 1, align 4
@falseOutput = private constant [30 x i8] c"False x is not an even number\00", align 1
@trueOutput = private constant [25 x i8] c"True x is an even number\00", align 1
@printedFormatBool = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@trueStr = private unnamed_addr constant [5 x i8] c"true\00", align 1
@falseStr = private unnamed_addr constant [6 x i8] c"false\00", align 1
@printedFormatInt = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@printedFormatBool.1 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@trueStr.2 = private unnamed_addr constant [5 x i8] c"true\00", align 1
@falseStr.3 = private unnamed_addr constant [6 x i8] c"false\00", align 1
@printedFormatBool.4 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@trueStr.5 = private unnamed_addr constant [5 x i8] c"true\00", align 1
@falseStr.6 = private unnamed_addr constant [6 x i8] c"false\00", align 1
@printedFormatString = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1

declare ptr @malloc(i64)

declare ptr @realloc(ptr, i64)

declare void @free(ptr)

define i1 @isEven() {
entry:
  %x = load i32, ptr @x, align 4
  %modtmp = srem i32 %x, 2
  store i32 %modtmp, ptr @y, align 4
  %0 = call i32 (ptr, ...) @printf(ptr @printedFormatBool, ptr @trueStr)
  %y = load i32, ptr @y, align 4
  %1 = call i32 (ptr, ...) @printf(ptr @printedFormatInt, i32 %y)
  %y1 = load i32, ptr @y, align 4
  %cmptmp = icmp ne i32 %y1, 0
  br i1 %cmptmp, label %then, label %else

then:                                             ; preds = %entry
  %2 = call i32 (ptr, ...) @printf(ptr @printedFormatBool.1, ptr @falseStr.3)
  ret i1 false

else:                                             ; preds = %entry
  %3 = call i32 (ptr, ...) @printf(ptr @printedFormatBool.4, ptr @trueStr.5)
  %4 = call i32 (ptr, ...) @printf(ptr @printedFormatString, ptr @trueOutput)
  ret i1 true
}

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %0 = call i1 @isEven()
  ret i32 0
}
