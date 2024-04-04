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
@printedFormatInt.1 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

; Function Attrs: nofree nounwind
define i32 @returnValidNumber() local_unnamed_addr #0 {
entry:
  %x = load i32, ptr @x, align 4
  %cmptmp = icmp eq i32 %x, 2
  br i1 %cmptmp, label %forBody, label %whileCond.preheader

whileCond.preheader:                              ; preds = %entry
  %cmptmp49 = icmp slt i32 %x, 11
  br i1 %cmptmp49, label %whileBody, label %common.ret

forBody:                                          ; preds = %entry, %forBody
  %loopVar.011 = phi i32 [ %nextVar, %forBody ], [ 1, %entry ]
  %t = load i32, ptr @t, align 4
  %addtmp = add i32 %t, 1
  store i32 %addtmp, ptr @t, align 4
  %0 = tail call i32 (ptr, ...) @printf(ptr nonnull dereferenceable(1) @printedFormatInt.1, i32 %addtmp)
  %nextVar = add nuw nsw i32 %loopVar.011, 1
  %exitcond.not = icmp eq i32 %nextVar, 10000000
  br i1 %exitcond.not, label %common.ret, label %forBody

common.ret:                                       ; preds = %whileBody, %forBody, %whileCond.preheader
  %common.ret.op.in = phi ptr [ @y, %whileCond.preheader ], [ @Q, %forBody ], [ @y, %whileBody ]
  %common.ret.op = load i32, ptr %common.ret.op.in, align 4
  ret i32 %common.ret.op

whileBody:                                        ; preds = %whileCond.preheader, %whileBody
  %x310 = phi i32 [ %addtmp7, %whileBody ], [ %x, %whileCond.preheader ]
  %1 = tail call i32 (ptr, ...) @printf(ptr nonnull dereferenceable(1) @printedFormatInt.1, i32 %x310)
  %x6 = load i32, ptr @x, align 4
  %addtmp7 = add i32 %x6, 1
  store i32 %addtmp7, ptr @x, align 4
  %cmptmp4 = icmp slt i32 %addtmp7, 11
  br i1 %cmptmp4, label %whileBody, label %common.ret
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
  %cmptmp49.i = icmp slt i32 %x.i, 11
  br i1 %cmptmp49.i, label %whileBody.i, label %returnValidNumber.exit

forBody.i:                                        ; preds = %entry, %forBody.i
  %loopVar.011.i = phi i32 [ %nextVar.i, %forBody.i ], [ 1, %entry ]
  %t.i = load i32, ptr @t, align 4
  %addtmp.i = add i32 %t.i, 1
  store i32 %addtmp.i, ptr @t, align 4
  %0 = tail call i32 (ptr, ...) @printf(ptr nonnull dereferenceable(1) @printedFormatInt.1, i32 %addtmp.i)
  %nextVar.i = add nuw nsw i32 %loopVar.011.i, 1
  %exitcond.not.i = icmp eq i32 %nextVar.i, 10000000
  br i1 %exitcond.not.i, label %returnValidNumber.exit, label %forBody.i

whileBody.i:                                      ; preds = %whileCond.preheader.i, %whileBody.i
  %x310.i = phi i32 [ %addtmp7.i, %whileBody.i ], [ %x.i, %whileCond.preheader.i ]
  %1 = tail call i32 (ptr, ...) @printf(ptr nonnull dereferenceable(1) @printedFormatInt.1, i32 %x310.i)
  %x6.i = load i32, ptr @x, align 4
  %addtmp7.i = add i32 %x6.i, 1
  store i32 %addtmp7.i, ptr @x, align 4
  %cmptmp4.i = icmp slt i32 %addtmp7.i, 11
  br i1 %cmptmp4.i, label %whileBody.i, label %returnValidNumber.exit

returnValidNumber.exit:                           ; preds = %whileBody.i, %forBody.i, %whileCond.preheader.i
  ret i32 0
}

attributes #0 = { nofree nounwind }
