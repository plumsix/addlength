$(EXE): $(OBJS)
	$(CC) $(CPPFLAGS) /Fe$*.exe $**

clean:
	del $(EXE) $(OBJS) $(EXE:exe=pdb)

