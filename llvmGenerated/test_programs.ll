; ModuleID = 'MyModule'
source_filename = "MyModule"

@q = local_unnamed_addr global float 0x3FD3333340000000, align 4
@isDeclared = local_unnamed_addr global i1 false, align 1
@x = local_unnamed_addr global i32 2, align 4
@y = local_unnamed_addr global i32 3, align 4
@p = local_unnamed_addr global i32 7, align 4
@Q = local_unnamed_addr global i32 9, align 4
@z = local_unnamed_addr global i32 0, align 4
@t = local_unnamed_addr global i32 2, align 4
@bb = local_unnamed_addr global i32 100, align 4
@printFormatInt.4 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

; Function Attrs: nofree nounwind
define i32 @returnValidNumber() local_unnamed_addr #0 {
entry:
  %bb = load i32, ptr @bb, align 4
  %0 = tail call i32 (ptr, ...) @printf(ptr nonnull dereferenceable(1) @printFormatInt.4, i32 %bb)
  %x = load i32, ptr @x, align 4
  %cmptmp = icmp eq i32 %x, 2
  br i1 %cmptmp, label %forBody, label %whileCond.preheader

whileCond.preheader:                              ; preds = %entry
  %cmptmp35 = icmp slt i32 %x, 10
  br i1 %cmptmp35, label %whileBody, label %common.ret

forBody:                                          ; preds = %entry
  %t = load i32, ptr @t, align 4
  %1 = tail call i32 (ptr, ...) @printf(ptr nonnull dereferenceable(1) @printFormatInt.4, i32 %t)
  br label %common.ret

common.ret:                                       ; preds = %whileBody, %whileCond.preheader, %forBody
  %common.ret.op.in = phi ptr [ @Q, %forBody ], [ @y, %whileCond.preheader ], [ @y, %whileBody ]
  %common.ret.op = load i32, ptr %common.ret.op.in, align 4
  ret i32 %common.ret.op

whileBody:                                        ; preds = %whileCond.preheader, %whileBody
  %x26 = phi i32 [ %x2.pr, %whileBody ], [ %x, %whileCond.preheader ]
  %2 = tail call i32 (ptr, ...) @printf(ptr nonnull dereferenceable(1) @printFormatInt.4, i32 %x26)
  %x2.pr = load i32, ptr @x, align 4
  %cmptmp3 = icmp slt i32 %x2.pr, 10
  br i1 %cmptmp3, label %whileBody, label %common.ret
}

; Function Attrs: nofree nounwind
declare noundef i32 @printf(ptr nocapture noundef readonly, ...) local_unnamed_addr #0

; Function Attrs: nofree nounwind
define noundef i32 @main() local_unnamed_addr #0 {
entry:
  %bb.i = load i32, ptr @bb, align 4
  %0 = tail call i32 (ptr, ...) @printf(ptr nonnull dereferenceable(1) @printFormatInt.4, i32 %bb.i)
  %x.i = load i32, ptr @x, align 4
  %cmptmp.i = icmp eq i32 %x.i, 2
  br i1 %cmptmp.i, label %forBody.i, label %whileCond.preheader.i

whileCond.preheader.i:                            ; preds = %entry
  %cmptmp35.i = icmp slt i32 %x.i, 10
  br i1 %cmptmp35.i, label %whileBody.i, label %returnValidNumber.exit

forBody.i:                                        ; preds = %entry
  %t.i = load i32, ptr @t, align 4
  %1 = tail call i32 (ptr, ...) @printf(ptr nonnull dereferenceable(1) @printFormatInt.4, i32 %t.i)
  br label %returnValidNumber.exit

whileBody.i:                                      ; preds = %whileCond.preheader.i, %whileBody.i
  %x26.i = phi i32 [ %x2.pr.i, %whileBody.i ], [ %x.i, %whileCond.preheader.i ]
  %2 = tail call i32 (ptr, ...) @printf(ptr nonnull dereferenceable(1) @printFormatInt.4, i32 %x26.i)
  %x2.pr.i = load i32, ptr @x, align 4
  %cmptmp3.i = icmp slt i32 %x2.pr.i, 10
  br i1 %cmptmp3.i, label %whileBody.i, label %returnValidNumber.exit

returnValidNumber.exit:                           ; preds = %whileBody.i, %whileCond.preheader.i, %forBody.i
  ret i32 0
}

attributes #0 = { nofree nounwind }
