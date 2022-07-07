install(DIRECTORY Tests/ DESTINATION include/EssaUtil/Tests FILES_MATCHING PATTERN "*.hpp")

essautil_add_test(UString LIBS essautil)
