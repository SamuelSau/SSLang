; ModuleID = 'MyModule'
source_filename = "MyModule"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@q = local_unnamed_addr global float 0x3FFF02E060000000, align 4
@isDeclared = local_unnamed_addr global i1 false, align 1
@x = local_unnamed_addr global i32 2, align 4
@y = local_unnamed_addr global i32 3, align 4
@p = local_unnamed_addr global i32 7, align 4
@Q = local_unnamed_addr global i32 9, align 4
@z = local_unnamed_addr global i32 0, align 4
@t = local_unnamed_addr global i32 0, align 4
@bb = local_unnamed_addr global i32 100, align 4
@cc = local_unnamed_addr global i32 10000000, align 4
@informalGreeting = private constant [4 x i8] c"sup\00", align 1
@formalGreeting = private constant [6 x i8] c"hello\00", align 1
@trueStr = private unnamed_addr constant [5 x i8] c"true\00", align 1
@falseStr = private unnamed_addr constant [6 x i8] c"false\00", align 1
@printedFormatInt = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

; Function Attrs: nofree nounwind
define noundef nonnull ptr @returnValidString() local_unnamed_addr #0 {
entry:
  %x = load i32, ptr @x, align 4
  %cmptmp = icmp eq i32 %x, 2
  br i1 %cmptmp, label %forBody, label %whileCond.preheader

whileCond.preheader:                              ; preds = %entry
  %cmptmp39 = icmp slt i32 %x, 11
  br i1 %cmptmp39, label %whileBody, label %common.ret

forBody:                                          ; preds = %entry
  %t = load i32, ptr @t, align 4
  %addtmp = add i32 %t, 1
  store i32 %addtmp, ptr @t, align 4
  %isDeclared = load i1, ptr @isDeclared, align 1
  %0 = select i1 %isDeclared, ptr @trueStr, ptr @falseStr
  %puts = tail call i32 @puts(ptr nonnull dereferenceable(1) %0)
  %puts7 = tail call i32 @puts(ptr nonnull dereferenceable(1) @informalGreeting)
  %puts8 = tail call i32 @puts(ptr nonnull dereferenceable(1) @informalGreeting)
  br label %common.ret

common.ret:                                       ; preds = %whileBody, %whileCond.preheader, %forBody
  %common.ret.op = phi ptr [ @informalGreeting, %forBody ], [ @formalGreeting, %whileCond.preheader ], [ @formalGreeting, %whileBody ]
  ret ptr %common.ret.op

whileBody:                                        ; preds = %whileCond.preheader, %whileBody
  %x210 = phi i32 [ %addtmp6, %whileBody ], [ %x, %whileCond.preheader ]
  %1 = tail call i32 (ptr, ...) @printf(ptr nonnull dereferenceable(1) @printedFormatInt, i32 %x210)
  %x5 = load i32, ptr @x, align 4
  %addtmp6 = add i32 %x5, 1
  store i32 %addtmp6, ptr @x, align 4
  %cmptmp3 = icmp slt i32 %addtmp6, 11
  br i1 %cmptmp3, label %whileBody, label %common.ret
}

; Function Attrs: nofree nounwind
declare noundef i32 @printf(ptr nocapture noundef readonly, ...) local_unnamed_addr #0

; Function Attrs: nofree nounwind
define noundef i32 @main() local_unnamed_addr #0 {
entry:
  %x.i = load i32, ptr @x, align 4
  %cmptmp.i = icmp eq i32 %x.i, 2
  br i1 %cmptmp.i, label %forBody.i, label %whileCond.preheader.i

whileCond.preheader.i:                            ; preds = %entry
  %cmptmp39.i = icmp slt i32 %x.i, 11
  br i1 %cmptmp39.i, label %whileBody.i, label %returnValidString.exit

forBody.i:                                        ; preds = %entry
  %t.i = load i32, ptr @t, align 4
  %addtmp.i = add i32 %t.i, 1
  store i32 %addtmp.i, ptr @t, align 4
  %isDeclared.i = load i1, ptr @isDeclared, align 1
  %0 = select i1 %isDeclared.i, ptr @trueStr, ptr @falseStr
  %puts.i = tail call i32 @puts(ptr nonnull dereferenceable(1) %0)
  %puts7.i = tail call i32 @puts(ptr nonnull dereferenceable(1) @informalGreeting)
  %puts8.i = tail call i32 @puts(ptr nonnull dereferenceable(1) @informalGreeting)
  br label %returnValidString.exit

whileBody.i:                                      ; preds = %whileCond.preheader.i, %whileBody.i
  %x210.i = phi i32 [ %addtmp6.i, %whileBody.i ], [ %x.i, %whileCond.preheader.i ]
  %1 = tail call i32 (ptr, ...) @printf(ptr nonnull dereferenceable(1) @printedFormatInt, i32 %x210.i)
  %x5.i = load i32, ptr @x, align 4
  %addtmp6.i = add i32 %x5.i, 1
  store i32 %addtmp6.i, ptr @x, align 4
  %cmptmp3.i = icmp slt i32 %addtmp6.i, 11
  br i1 %cmptmp3.i, label %whileBody.i, label %returnValidString.exit

returnValidString.exit:                           ; preds = %whileBody.i, %whileCond.preheader.i, %forBody.i
  ret i32 0
}

; Function Attrs: nofree nounwind
declare noundef i32 @puts(ptr nocapture noundef readonly) local_unnamed_addr #0

attributes #0 = { nofree nounwind }
