install(DIRECTORY Tests/ DESTINATION include/EssaUtil/Tests FILES_MATCHING PATTERN "*.hpp")

essautil_add_test(Matrix LIBS essautil)
essautil_add_test(Stream LIBS essautil)
essautil_add_test(UString LIBS essautil)
essautil_add_test(UnitDisplay LIBS essautil)
essautil_add_test(Vector LIBS essautil)
