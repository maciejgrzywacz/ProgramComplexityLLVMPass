; ModuleID = 'index_is_input.c'
source_filename = "index_is_input.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@global_array = dso_local local_unnamed_addr global [10 x i32] zeroinitializer, align 16, !dbg !0
@.str = private unnamed_addr constant [3 x i8] c"%d\00", align 1, !dbg !5

; Function Attrs: nofree nounwind uwtable
define dso_local i32 @foo(i32 noundef %a, i32 noundef %b, i32 noundef %limit) local_unnamed_addr #0 !dbg !23 {
entry:
  %c = alloca i32, align 4, !DIAssignID !33
  call void @llvm.dbg.assign(metadata i1 undef, metadata !30, metadata !DIExpression(), metadata !33, metadata ptr %c, metadata !DIExpression()), !dbg !34
  tail call void @llvm.dbg.value(metadata i32 %a, metadata !27, metadata !DIExpression()), !dbg !34
  tail call void @llvm.dbg.value(metadata i32 %b, metadata !28, metadata !DIExpression()), !dbg !34
  tail call void @llvm.dbg.value(metadata i32 %limit, metadata !29, metadata !DIExpression()), !dbg !34
  call void @llvm.lifetime.start.p0(i64 4, ptr nonnull %c), !dbg !35
  store volatile i32 9, ptr %c, align 4, !dbg !36, !tbaa !37, !DIAssignID !41
  call void @llvm.dbg.assign(metadata i32 9, metadata !30, metadata !DIExpression(), metadata !41, metadata ptr %c, metadata !DIExpression()), !dbg !34
  %cmp26 = icmp slt i32 %a, %limit, !dbg !42
  br i1 %cmp26, label %for.cond1.preheader, label %for.end7, !dbg !45

for.cond1.preheader:                              ; preds = %entry, %for.inc5
  %a.addr.028 = phi i32 [ %inc6, %for.inc5 ], [ %a, %entry ]
  %b.addr.027 = phi i32 [ %b.addr.1.lcssa, %for.inc5 ], [ %b, %entry ]
  tail call void @llvm.dbg.value(metadata i32 %a.addr.028, metadata !27, metadata !DIExpression()), !dbg !34
  tail call void @llvm.dbg.value(metadata i32 %b.addr.027, metadata !28, metadata !DIExpression()), !dbg !34
  %b.addr.123 = add nsw i32 %b.addr.027, 1, !dbg !46
  tail call void @llvm.dbg.value(metadata i32 %b.addr.123, metadata !28, metadata !DIExpression()), !dbg !34
  %cmp224 = icmp slt i32 %b.addr.123, %limit, !dbg !48
  br i1 %cmp224, label %for.body3, label %for.inc5, !dbg !51

for.body3:                                        ; preds = %for.cond1.preheader, %for.body3
  %b.addr.125 = phi i32 [ %b.addr.1, %for.body3 ], [ %b.addr.123, %for.cond1.preheader ]
  %c.0.c.0.c.0.c.0. = load volatile i32, ptr %c, align 4, !dbg !52, !tbaa !37
  %add4 = add nsw i32 %c.0.c.0.c.0.c.0., 1, !dbg !52
  store volatile i32 %add4, ptr %c, align 4, !dbg !52, !tbaa !37, !DIAssignID !54
  call void @llvm.dbg.assign(metadata i32 %add4, metadata !30, metadata !DIExpression(), metadata !54, metadata ptr %c, metadata !DIExpression()), !dbg !34
  tail call void @llvm.dbg.value(metadata i32 %b.addr.125, metadata !28, metadata !DIExpression(DW_OP_plus_uconst, 1, DW_OP_stack_value)), !dbg !34
  %b.addr.1 = add i32 %b.addr.125, 1, !dbg !46
  tail call void @llvm.dbg.value(metadata i32 %b.addr.1, metadata !28, metadata !DIExpression()), !dbg !34
  %exitcond.ntests/loop/index_is_input_O1.ll tests/loop/index_is_input_O1_no_debug.ll = icmp eq i32 %b.addr.1, %limit, !dbg !48
  br i1 %exitcond.not, label %for.inc5, label %for.body3, !dbg !51, !llvm.loop !55

for.inc5:                                         ; preds = %for.body3, %for.cond1.preheader
  %b.addr.1.lcssa = phi i32 [ %b.addr.123, %for.cond1.preheader ], [ %limit, %for.body3 ], !dbg !46
  %inc6 = add i32 %a.addr.028, 1, !dbg !59
  tail call void @llvm.dbg.value(metadata i32 %inc6, metadata !27, metadata !DIExpression()), !dbg !34
  tail call void @llvm.dbg.value(metadata i32 %b.addr.1.lcssa, metadata !28, metadata !DIExpression()), !dbg !34
  %exitcond31.not = icmp eq i32 %inc6, %limit, !dbg !42
  br i1 %exitcond31.not, label %for.end7, label %for.cond1.preheader, !dbg !45, !llvm.loop !60

for.end7:                                         ; preds = %for.inc5, %entry
  %a.addr.0.lcssa = phi i32 [ %a, %entry ], [ %limit, %for.inc5 ]
  %c.0.c.0.c.0.c.0.16 = load volatile i32, ptr %c, align 4, !dbg !62, !tbaa !37
  %cmp8 = icmp sgt i32 %c.0.c.0.c.0.c.0.16, 36, !dbg !64
  br i1 %cmp8, label %if.then, label %if.else, !dbg !65

if.then:                                          ; preds = %for.end7
  %rem = srem i32 %a.addr.0.lcssa, 10, !dbg !66
  %idxprom = sext i32 %rem to i64, !dbg !68
  %arrayidx = getelementptr inbounds [10 x i32], ptr @global_array, i64 0, i64 %idxprom, !dbg !68
  %0 = load i32, ptr %arrayidx, align 4, !dbg !68, !tbaa !37
  tail call void @llvm.dbg.value(metadata i32 %0, metadata !32, metadata !DIExpression()), !dbg !34
  %call = tail call i32 (ptr, ...) @printf(ptr noundef nonnull dereferenceable(1) @.str, i32 noundef %0), !dbg !69
  br label %if.end, !dbg !70

if.else:                                          ; preds = %for.end7
  %rem9 = srem i32 %limit, 10, !dbg !71
  %idxprom10 = sext i32 %rem9 to i64, !dbg !73
  %arrayidx11 = getelementptr inbounds [10 x i32], ptr @global_array, i64 0, i64 %idxprom10, !dbg !73
  %1 = load i32, ptr %arrayidx11, align 4, !dbg !73, !tbaa !37
  tail call void @llvm.dbg.value(metadata i32 %1, metadata !32, metadata !DIExpression()), !dbg !34
  %conv = sitofp i32 %1 to double, !dbg !74
  %call12 = tail call double @log(double noundef %conv) #6, !dbg !75
  %conv13 = fptosi double %call12 to i32, !dbg !75
  tail call void @llvm.dbg.value(metadata i32 %conv13, metadata !32, metadata !DIExpression()), !dbg !34
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %return_value.0 = phi i32 [ %0, %if.then ], [ %conv13, %if.else ], !dbg !76
  tail call void @llvm.dbg.value(metadata i32 %return_value.0, metadata !32, metadata !DIExpression()), !dbg !34
  call void @llvm.lifetime.end.p0(i64 4, ptr nonnull %c), !dbg !77
  ret i32 %return_value.0, !dbg !78
}

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.start.p0(i64 immarg, ptr nocapture) #1

; Function Attrs: nofree nounwind
declare !dbg !79 noundef i32 @printf(ptr nocapture noundef readonly, ...) local_unnamed_addr #2

; Function Attrs: mustprogress nofree nounwind willreturn memory(write)
declare !dbg !86 double @log(double noundef) local_unnamed_addr #3

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.end.p0(i64 immarg, ptr nocapture) #1

; Function Attrs: mustprogress nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare void @llvm.dbg.assign(metadata, metadata, metadata, metadata, metadata, metadata) #4

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare void @llvm.dbg.value(metadata, metadata, metadata) #5

attributes #0 = { nofree nounwind uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite) }
attributes #2 = { nofree nounwind "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #3 = { mustprogress nofree nounwind willreturn memory(write) "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #4 = { mustprogress nocallback nofree nosync nounwind speculatable willreturn memory(none) }
attributes #5 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }
attributes #6 = { nounwind }

!llvm.dbg.cu = !{!2}
!llvm.module.flags = !{!15, !16, !17, !18, !19, !20, !21}
!llvm.ident = !{!22}

!0 = !DIGlobalVariableExpression(var: !1, expr: !DIExpression())
!1 = distinct !DIGlobalVariable(name: "global_array", scope: !2, file: !3, line: 4, type: !11, isLocal: false, isDefinition: true)
!2 = distinct !DICompileUnit(language: DW_LANG_C11, file: !3, producer: "clang version 18.0.0git (https://github.com/llvm/llvm-project.git 86bc18ade8f335f03f607142311957129e156efc)", isOptimized: true, runtimeVersion: 0, emissionKind: FullDebug, globals: !4, splitDebugInlining: false, nameTableKind: None)
!3 = !DIFile(filename: "index_is_input.c", directory: "/home/ubuntu/mgr/tests/loop", checksumkind: CSK_MD5, checksum: "7c38b6524b93fb2f3027c3f0816e7bfc")
!4 = !{!0, !5}
!5 = !DIGlobalVariableExpression(var: !6, expr: !DIExpression())
!6 = distinct !DIGlobalVariable(scope: null, file: !3, line: 18, type: !7, isLocal: true, isDefinition: true)
!7 = !DICompositeType(tag: DW_TAG_array_type, baseType: !8, size: 24, elements: !9)
!8 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!9 = !{!10}
!10 = !DISubrange(count: 3)
!11 = !DICompositeType(tag: DW_TAG_array_type, baseType: !12, size: 320, elements: !13)
!12 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!13 = !{!14}
!14 = !DISubrange(count: 10)
!15 = !{i32 7, !"Dwarf Version", i32 5}
!16 = !{i32 2, !"Debug Info Version", i32 3}
!17 = !{i32 1, !"wchar_size", i32 4}
!18 = !{i32 8, !"PIC Level", i32 2}
!19 = !{i32 7, !"PIE Level", i32 2}
!20 = !{i32 7, !"uwtable", i32 2}
!21 = !{i32 7, !"debug-info-assignment-tracking", i1 true}
!22 = !{!"clang version 18.0.0git (https://github.com/llvm/llvm-project.git 86bc18ade8f335f03f607142311957129e156efc)"}
!23 = distinct !DISubprogram(name: "foo", scope: !3, file: !3, line: 6, type: !24, scopeLine: 6, flags: DIFlagPrototyped | DIFlagAllCallsDescribed, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !2, retainedNodes: !26)
!24 = !DISubroutineType(types: !25)
!25 = !{!12, !12, !12, !12}
!26 = !{!27, !28, !29, !30, !32}
!27 = !DILocalVariable(name: "a", arg: 1, scope: !23, file: !3, line: 6, type: !12)
!28 = !DILocalVariable(name: "b", arg: 2, scope: !23, file: !3, line: 6, type: !12)
!29 = !DILocalVariable(name: "limit", arg: 3, scope: !23, file: !3, line: 6, type: !12)
!30 = !DILocalVariable(name: "c", scope: !23, file: !3, line: 7, type: !31)
!31 = !DIDerivedType(tag: DW_TAG_volatile_type, baseType: !12)
!32 = !DILocalVariable(name: "return_value", scope: !23, file: !3, line: 15, type: !12)
!33 = distinct !DIAssignID()
!34 = !DILocation(line: 0, scope: !23)
!35 = !DILocation(line: 7, column: 5, scope: !23)
!36 = !DILocation(line: 7, column: 18, scope: !23)
!37 = !{!38, !38, i64 0}
!38 = !{!"int", !39, i64 0}
!39 = !{!"omnipotent char", !40, i64 0}
!40 = !{!"Simple C/C++ TBAA"}
!41 = distinct !DIAssignID()
!42 = !DILocation(line: 8, column: 13, scope: !43)
!43 = distinct !DILexicalBlock(scope: !44, file: !3, line: 8, column: 5)
!44 = distinct !DILexicalBlock(scope: !23, file: !3, line: 8, column: 5)
!45 = !DILocation(line: 8, column: 5, scope: !44)
!46 = !DILocation(line: 0, scope: !47)
!47 = distinct !DILexicalBlock(scope: !43, file: !3, line: 8, column: 27)
!48 = !DILocation(line: 10, column: 17, scope: !49)
!49 = distinct !DILexicalBlock(scope: !50, file: !3, line: 10, column: 9)
!50 = distinct !DILexicalBlock(scope: !47, file: !3, line: 10, column: 9)
!51 = !DILocation(line: 10, column: 9, scope: !50)
!52 = !DILocation(line: 11, column: 15, scope: !53)
!53 = distinct !DILexicalBlock(scope: !49, file: !3, line: 10, column: 31)
!54 = distinct !DIAssignID()
!55 = distinct !{!55, !51, !56, !57, !58}
!56 = !DILocation(line: 12, column: 9, scope: !50)
!57 = !{!"llvm.loop.mustprogress"}
!58 = !{!"llvm.loop.unroll.disable"}
!59 = !DILocation(line: 8, column: 23, scope: !43)
!60 = distinct !{!60, !45, !61, !57, !58}
!61 = !DILocation(line: 13, column: 5, scope: !44)
!62 = !DILocation(line: 16, column: 9, scope: !63)
!63 = distinct !DILexicalBlock(scope: !23, file: !3, line: 16, column: 9)
!64 = !DILocation(line: 16, column: 11, scope: !63)
!65 = !DILocation(line: 16, column: 9, scope: !23)
!66 = !DILocation(line: 17, column: 39, scope: !67)
!67 = distinct !DILexicalBlock(scope: !63, file: !3, line: 16, column: 17)
!68 = !DILocation(line: 17, column: 24, scope: !67)
!69 = !DILocation(line: 18, column: 9, scope: !67)
!70 = !DILocation(line: 19, column: 5, scope: !67)
!71 = !DILocation(line: 21, column: 43, scope: !72)
!72 = distinct !DILexicalBlock(scope: !63, file: !3, line: 20, column: 10)
!73 = !DILocation(line: 21, column: 24, scope: !72)
!74 = !DILocation(line: 22, column: 28, scope: !72)
!75 = !DILocation(line: 22, column: 24, scope: !72)
!76 = !DILocation(line: 0, scope: !63)
!77 = !DILocation(line: 26, column: 1, scope: !23)
!78 = !DILocation(line: 25, column: 5, scope: !23)
!79 = !DISubprogram(name: "printf", scope: !80, file: !80, line: 356, type: !81, flags: DIFlagPrototyped, spFlags: DISPFlagOptimized)
!80 = !DIFile(filename: "/usr/include/stdio.h", directory: "", checksumkind: CSK_MD5, checksum: "f31eefcc3f15835fc5a4023a625cf609")
!81 = !DISubroutineType(types: !82)
!82 = !{!12, !83, null}
!83 = !DIDerivedType(tag: DW_TAG_restrict_type, baseType: !84)
!84 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !85, size: 64)
!85 = !DIDerivedType(tag: DW_TAG_const_type, baseType: !8)
!86 = !DISubprogram(name: "log", scope: !87, file: !87, line: 104, type: !88, flags: DIFlagPrototyped, spFlags: DISPFlagOptimized)
!87 = !DIFile(filename: "/usr/include/x86_64-linux-gnu/bits/mathcalls.h", directory: "", checksumkind: CSK_MD5, checksum: "8c6e2d0d2bda65bc5ba1ca02b65383b7")
!88 = !DISubroutineType(types: !89)
!89 = !{!90, !90}
!90 = !DIBasicType(name: "double", size: 64, encoding: DW_ATE_float)
