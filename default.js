function change_zoom(v) {
    if(v > 0 && user2 > 0)
        user2++;
    else if(v < 0 && user2 < 0)
        user2--;
    else if(v > 0 && user2 < 0)
        user2 = 1;
    else if(v < 0 && user2 > 0)
        user2 = -1;
}