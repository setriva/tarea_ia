.PHONY: clean All

All:
	@echo "----------Building project:[ mrp - Debug ]----------"
	@$(MAKE) -f  "mrp.mk"
clean:
	@echo "----------Cleaning project:[ mrp - Debug ]----------"
	@$(MAKE) -f  "mrp.mk" clean
