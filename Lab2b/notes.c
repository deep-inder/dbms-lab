// for put-rec with indexing, we will need the location of the file pointer

//fseek (fp, 0, SEEK_END);
//offset = ftell(fp); -> To find the location of the file pointer
//fwrite(fp, ...)
//index.key = key;
// index.location = offset;
// insert index to bst


// get-rec
    //Search for key in BST
    //Identify offset of record
    //fseek(fp, offset, SEEK_SET)
    //fread(...)