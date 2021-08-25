DST=

copy_tests: ticker/tests concat/tests
	if [ "" = "${DST}" ]; then exit 1; fi
	cp -rf ticker/tests ticker/Makefile ${DST}/ticker/
	cp -rf concat/tests concat/Makefile ${DST}/concat
	cp -rf Makefile ${DST}/

test:
	@rm -f failed
	@make -C concat test || echo $$? concat_failed > failed
	@make -C ticker test || echo $$? ticker_failed >>failed
	@if [ -s failed ]; then exit 1; fi

.PHONY: clean test copy_tests
