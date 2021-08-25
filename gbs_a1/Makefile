DST=

copy_tests: lilo/tests warmup/tests
	if [ "" = "${DST}" ]; then exit 1; fi
	cp -rf lilo/tests lilo/Makefile ${DST}/lilo/
	cp -rf warmup/tests warmup/Makefile ${DST}/warmup
	cp -rf Makefile ${DST}/

test:
	@rm -f failed
	@make -C warmup test || echo $$? warmup_failed > failed
	@make -C lilo test || echo $$? lilo_failed >>failed
	@if [ -s failed ]; then exit 1; fi

.PHONY: clean test copy_tests
