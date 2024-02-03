; ModuleID = 'index_is_input.c'
source_filename = "index_is_input.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@global_array = dso_local local_unnamed_addr global [10 x i32] zeroinitializer, align 16
@.str = private unnamed_addr constant [3 x i8] c"%d\00", align 1

; Function Attrs: nofree nounwind uwtable
define dso_local i32 @foo(i32 noundef %a, i32 noundef %b, i32 noundef %limit) local_unnamed_addr #0 {
entry:
  %c = alloca i32, align 4
  call void @llvm.lifetime.start.p0(i64 4, ptr nonnull %c)
  store volatile i32 9, ptr %c, align 4, !tbaa !5
  %cmp26 = icmp slt i32 %a, %limit
  br i1 %cmp26, label %for.cond1.preheader, label %for.end7

for.cond1.preheader:                              ; preds = %entry, %for.inc5
  %a.addr.028 = phi i32 [ %inc6, %for.inc5 ], [ %a, %entry ]
  %b.addr.027 = phi i32 [ %b.addr.1.lcssa, %for.inc5 ], [ %b, %entry ]
  %b.addr.123 = add nsw i32 %b.addr.027, 1
  %cmp224 = icmp slt i32 %b.addr.123, %limit
  br i1 %cmp224, label %for.body3, label %for.inc5

for.body3:                                        ; preds = %for.cond1.preheader, %for.body3
  %b.addr.125 = phi i32 [ %b.addr.1, %for.body3 ], [ %b.addr.123, %for.cond1.preheader ]
  %c.0.c.0.c.0.c.0. = load volatile i32, ptr %c, align 4, !tbaa !5
  %add4 = add nsw i32 %c.0.c.0.c.0.c.0., 1
  store volatile i32 %add4, ptr %c, align 4, !tbaa !5
  %b.addr.1 = add i32 %b.addr.125, 1
  %exitcond.not = icmp eq i32 %b.addr.1, %limit
  br i1 %exitcond.not, label %for.inc5, label %for.body3, !llvm.loop !9

for.inc5:                                         ; preds = %for.body3, %for.cond1.preheader
  %b.addr.1.lcssa = phi i32 [ %b.addr.123, %for.cond1.preheader ], [ %limit, %for.body3 ]
  %inc6 = add i32 %a.addr.028, 1
  %exitcond31.not = icmp eq i32 %inc6, %limit
  br i1 %exitcond31.not, label %for.end7, label %for.cond1.preheader, !llvm.loop !12

for.end7:                                         ; preds = %for.inc5, %entry
  %a.addr.0.lcssa = phi i32 [ %a, %entry ], [ %limit, %for.inc5 ]
  %c.0.c.0.c.0.c.0.16 = load volatile i32, ptr %c, align 4, !tbaa !5
  %cmp8 = icmp sgt i32 %c.0.c.0.c.0.c.0.16, 36
  br i1 %cmp8, label %if.then, label %if.else

if.then:                                          ; preds = %for.end7
  %rem = srem i32 %a.addr.0.lcssa, 10
  %idxprom = sext i32 %rem to i64
  %arrayidx = getelementptr inbounds [10 x i32], ptr @global_array, i64 0, i64 %idxprom
  %0 = load i32, ptr %arrayidx, align 4, !tbaa !5
  %call = tail call i32 (ptr, ...) @printf(ptr noundef nonnull dereferenceable(1) @.str, i32 noundef %0)
  br label %if.end

if.else:                                          ; preds = %for.end7
  %rem9 = srem i32 %limit, 10
  %idxprom10 = sext i32 %rem9 to i64
  %arrayidx11 = getelementptr inbounds [10 x i32], ptr @global_array, i64 0, i64 %idxprom10
  %1 = load i32, ptr %arrayidx11, align 4, !tbaa !5
  %conv = sitofp i32 %1 to double
  %call12 = tail call double @log(double noundef %conv) #4
  %conv13 = fptosi double %call12 to i32
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %return_value.0 = phi i32 [ %0, %if.then ], [ %conv13, %if.else ]
  call void @llvm.lifetime.end.p0(i64 4, ptr nonnull %c)
  ret i32 %return_value.0
}

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.start.p0(i64 immarg, ptr nocapture) #1

; Function Attrs: nofree nounwind
declare noundef i32 @printf(ptr nocapture noundef readonly, ...) local_unnamed_addr #2

; Function Attrs: mustprogress nofree nounwind willreturn memory(write)
declare double @log(double noundef) local_unnamed_addr #3

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.end.p0(i64 immarg, ptr nocapture) #1

attributes #0 = { nofree nounwind uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite) }
attributes #2 = { nofree nounwind "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #3 = { mustprogress nofree nounwind willreturn memory(write) "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #4 = { nounwind }

!llvm.module.flags = !{!0, !1, !2, !3}
!llvm.ident = !{!4}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{!"clang version 18.0.0git (https://github.com/llvm/llvm-project.git 86bc18ade8f335f03f607142311957129e156efc)"}
!5 = !{!6, !6, i64 0}
!6 = !{!"int", !7, i64 0}
!7 = !{!"omnipotent char", !8, i64 0}
!8 = !{!"Simple C/C++ TBAA"}
!9 = distinct !{!9, !10, !11}
!10 = !{!"llvm.loop.mustprogress"}
!11 = !{!"llvm.loop.unroll.disable"}
!12 = distinct !{!12, !10, !11}
