PARAMGEN=tools/param-gen/paramgen.py

paramwatch:
	@python tools/param-gen/paramwatch.py

paramgen:
	@python $(PARAMGEN) $(filter-out $@,$(MAKECMDGOALS))

%:
	@:
