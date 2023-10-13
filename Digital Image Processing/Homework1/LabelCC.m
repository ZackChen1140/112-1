function result = LabelCC(img, num)
% component labeling with num-connected (num =4 or 8)
    M = logical(img);
    [m, n] = size(M);
    vL = [];
    L = 0;
    if num == 4
        % ------ implement your 4-connected components labeling algorithm here ------
        % result = your answer 
        tM = zeros(m + 1, n + 1);
        tM(2 : end, 2 : end) = M;
        for j = 2 : (n + 1)
            for i = 2 : (m + 1)
                 if tM(i, j) == 1
                     r = tM(i - 1, j);
                     t = tM(i, j - 1);
                     if r + t == 0 %皆0
                         L = L + 1;
                         vL(end + 1) = L;
                         tM(i, j) = L;
                     elseif r > 0 && t > 0 && r ~= t %皆非0且不相等
                         tM(i, j) = min([r, t]);
                         tM(tM == max([r, t])) = min([r, t]);
                         idx = find(vL == max([r, t]));
                         vL(idx) = [];
                         if idx <= length(vL)
                             tM(tM == vL(idx)) = idx;
                         end
                         L = vL(end);
                     else
                         tM(i, j) = max([r, t]);
                     end
                 end
            end
        end
        tM = tM(2 : end, 2 : end);
        result = tM;
    elseif num ==8
        % ------ implement your 8-connected components labeling algorithm here ------
        % result = your answer 
        tM = zeros(m + 2, n + 2);
        tM(2 : end - 1, 2 : end - 1) = M;
        for j = 2 : (n + 1)
            for i = 2 : (m + 1)
                 if tM(i, j) == 1
                     q = tM(i - 1, j - 1);
                     r = tM(i - 1, j);
                     s = tM(i - 1, j + 1);
                     t = tM(i, j - 1);
                     u = tM(i + 1, j - 1);
                     sArray = sort([q, r, s, t, u], 'descend');
                     lnzIdx = length(sArray);
                     if ~isempty(find(sArray == 0, 1))
                         lnzIdx = find(sArray == 0, 1) - 1;
                     end
                     if (q + r + s + t + u) == 0 %皆0
                         L = L + 1;
                         vL(end + 1) = L;
                         tM(i, j) = L;
                     else %部分不為0
                         tM(i, j) = sArray(lnzIdx);
                         for k = 1 : (lnzIdx - 1)
                             if sArray(k) ~= sArray(lnzIdx)
                                 tM(tM == sArray(k)) = sArray(lnzIdx);
                                 idx = find(vL == sArray(k), 1);
                                 vL(idx) = [];
                                 if idx <= length(vL)
                                     tM(tM == vL(idx)) = idx;
                                 end
                                 L = vL(end);
                             end
                         end
                     end
                 end
            end
        end
        tM = tM(2 : end - 1, 2 : end - 1);
        result = tM;
    else
        error('MyFunc:InputMustbe4or8','The second argument should be 4 or 8.')
    end
end
