; ModuleID = 'MyModule'
source_filename = "MyModule"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@x = local_unnamed_addr global i32 10, align 4
@y = local_unnamed_addr global i32 1, align 4
@trueOutput = private constant [25 x i8] c"True x is an even number\00", align 1
@printedFormatInt = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@falseStr.3 = private unnamed_addr constant [6 x i8] c"false\00", align 1
@trueStr.5 = private unnamed_addr constant [5 x i8] c"true\00", align 1

; Function Attrs: nofree nounwind
define noundef i1 @isEven() local_unnamed_addr #0 {
entry:
  %x = load i32, ptr @x, align 4
  %modtmp = srem i32 %x, 2
  store i32 %modtmp, ptr @y, align 4
  %puts = tail call i32 @puts(ptr nonnull dereferenceable(1) @trueStr.5)
  %y = load i32, ptr @y, align 4
  %0 = tail call i32 (ptr, ...) @printf(ptr nonnull dereferenceable(1) @printedFormatInt, i32 %y)
  %y1 = load i32, ptr @y, align 4
  %cmptmp.not = icmp eq i32 %y1, 0
  br i1 %cmptmp.not, label %else, label %common.ret

common.ret:                                       ; preds = %entry, %else
  %trueOutput.sink = phi ptr [ @trueOutput, %else ], [ @falseStr.3, %entry ]
  %puts3 = tail call i32 @puts(ptr nonnull dereferenceable(1) %trueOutput.sink)
  ret i1 %cmptmp.not

else:                                             ; preds = %entry
  %puts2 = tail call i32 @puts(ptr nonnull dereferenceable(1) @trueStr.5)
  br label %common.ret
}

; Function Attrs: nofree nounwind
declare noundef i32 @printf(ptr nocapture noundef readonly, ...) local_unnamed_addr #0

; Function Attrs: nofree nounwind
define noundef i32 @main() local_unnamed_addr #0 {
entry:
  %x.i = load i32, ptr @x, align 4
  %modtmp.i = srem i32 %x.i, 2
  store i32 %modtmp.i, ptr @y, align 4
  %puts.i = tail call i32 @puts(ptr nonnull dereferenceable(1) @trueStr.5)
  %y.i = load i32, ptr @y, align 4
  %0 = tail call i32 (ptr, ...) @printf(ptr nonnull dereferenceable(1) @printedFormatInt, i32 %y.i)
  %y1.i = load i32, ptr @y, align 4
  %cmptmp.not.i = icmp eq i32 %y1.i, 0
  br i1 %cmptmp.not.i, label %else.i, label %isEven.exit

else.i:                                           ; preds = %entry
  %puts2.i = tail call i32 @puts(ptr nonnull dereferenceable(1) @trueStr.5)
  br label %isEven.exit

isEven.exit:                                      ; preds = %entry, %else.i
  %trueOutput.sink.i = phi ptr [ @trueOutput, %else.i ], [ @falseStr.3, %entry ]
  %puts3.i = tail call i32 @puts(ptr nonnull dereferenceable(1) %trueOutput.sink.i)
  ret i32 0
}

; Function Attrs: nofree nounwind
declare noundef i32 @puts(ptr nocapture noundef readonly) local_unnamed_addr #0

attributes #0 = { nofree nounwind }
